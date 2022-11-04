#include <Python.h>

static PyObject *tqdmc_print(PyObject *self, PyObject *args) {
  const char *command;

  if (!PyArg_ParseTuple(args, "s", &command))
    return NULL;

  write(1, command, strlen(command));

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *tqdmc_count(PyObject *self, PyObject *args) {
  PyObject *obj;
  int count = 0;

  if (!PyArg_ParseTuple(args, "O", &obj))
    return NULL;

  PyObject *iterator = PyObject_GetIter(obj);
  PyObject *item;

  if (iterator == NULL) {
    return NULL;
  }

  while ((item = PyIter_Next(iterator))) {
    count++;
    Py_DECREF(item);
  }

  Py_DECREF(iterator);

  return PyLong_FromLong(count);
}

typedef struct {
  PyObject_HEAD Py_ssize_t seq_index, enum_index;
  PyObject *iterable;
} YielderState;

static PyObject *yielder_new(PyTypeObject *type, PyObject *args,
                             PyObject *kwargs) {
  PyObject *obj;

  if (!PyArg_UnpackTuple(args, "yielder", 1, 1, &obj))
    return NULL;

  YielderState *yielder = (YielderState *)type->tp_alloc(type, 0);
  if (!yielder)
    return NULL;

  PyObject *iterable = PyObject_GetIter(obj);
  if (!iterable)
    return NULL;

  Py_INCREF(iterable);
  yielder->iterable = iterable;

  return (PyObject *)yielder;
}

static void yielder_dealloc(YielderState *yielder) {
  /* We need XDECREF here because when the generator is exhausted,
   * rgstate->sequence is cleared with Py_CLEAR which sets it to NULL.
   */
  Py_XDECREF(yielder->iterable);
  Py_TYPE(yielder)->tp_free(yielder);
}

static PyObject *yielder_next(YielderState *yielder) {
  PyObject *item = PyIter_Next(yielder->iterable);
  return item;
}

PyTypeObject PyYielder_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0) "yielder", /* tp_name */
    sizeof(YielderState),                             /* tp_basicsize */
    0,                                                /* tp_itemsize */
    (destructor)yielder_dealloc,                      /* tp_dealloc */
    0,                                                /* tp_print */
    0,                                                /* tp_getattr */
    0,                                                /* tp_setattr */
    0,                                                /* tp_reserved */
    0,                                                /* tp_repr */
    0,                                                /* tp_as_number */
    0,                                                /* tp_as_sequence */
    0,                                                /* tp_as_mapping */
    0,                                                /* tp_hash */
    0,                                                /* tp_call */
    0,                                                /* tp_str */
    0,                                                /* tp_getattro */
    0,                                                /* tp_setattro */
    0,                                                /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                               /* tp_flags */
    0,                                                /* tp_doc */
    0,                                                /* tp_traverse */
    0,                                                /* tp_clear */
    0,                                                /* tp_richcompare */
    0,                                                /* tp_weaklistoffset */
    PyObject_SelfIter,                                /* tp_iter */
    (iternextfunc)yielder_next,                       /* tp_iternext */
    0,                                                /* tp_methods */
    0,                                                /* tp_members */
    0,                                                /* tp_getset */
    0,                                                /* tp_base */
    0,                                                /* tp_dict */
    0,                                                /* tp_descr_get */
    0,                                                /* tp_descr_set */
    0,                                                /* tp_dictoffset */
    0,                                                /* tp_init */
    PyType_GenericAlloc,                              /* tp_alloc */
    yielder_new,                                      /* tp_new */
};

typedef struct {
  PyObject_HEAD Py_ssize_t seq_index, enum_index;
  PyObject *tqdm;
  PyObject *iter;
  double mininterval;
  double last_print_t;
  long last_print_n;
  double min_start_t;
  long n;
  PyObject *update;
} TqdmCIterState;

static PyObject *tqdmciter_new(PyTypeObject *type, PyObject *args,
                               PyObject *kwargs) {
  PyObject *tqdm;

  if (!PyArg_UnpackTuple(args, "tqdmciter", 1, 1, &tqdm))
    return NULL;

  TqdmCIterState *tqdmciter = (TqdmCIterState *)type->tp_alloc(type, 0);
  if (!tqdmciter)
    return NULL;
  tqdmciter->tqdm = tqdm;

  PyObject *iterable = PyObject_GetAttrString(tqdm, "iterable");
  if (!iterable)
    return NULL;

  PyObject *iter = PyObject_GetIter(iterable);
  if (!iter)
    return NULL;

  Py_INCREF(iter);
  tqdmciter->iter = iter;

  PyObject *mininterval = PyObject_GetAttrString(tqdm, "mininterval");
  if (!mininterval || !PyFloat_Check(mininterval))
    return NULL;
  tqdmciter->mininterval = PyFloat_AsDouble(mininterval);

  PyObject *last_print_t = PyObject_GetAttrString(tqdm, "last_print_t");
  if (!last_print_t || !PyFloat_Check(last_print_t))
    return NULL;
  tqdmciter->last_print_t = PyFloat_AsDouble(last_print_t);

  PyObject *last_print_n = PyObject_GetAttrString(tqdm, "last_print_n");
  if (!last_print_n || !PyLong_Check(last_print_n))
    return NULL;
  tqdmciter->last_print_n = PyLong_AsLong(last_print_n);

  PyObject *start_t = PyObject_GetAttrString(tqdm, "start_t");
  if (!last_print_t || !PyFloat_Check(start_t))
    return NULL;
  PyObject *delay = PyObject_GetAttrString(tqdm, "delay");
  if (!delay)
    return NULL;
  tqdmciter->min_start_t = PyFloat_AsDouble(PyNumber_Add(start_t, delay));

  PyObject *n = PyObject_GetAttrString(tqdm, "n");
  if (!n || !PyLong_Check(last_print_n))
    return NULL;
  tqdmciter->min_start_t = PyLong_AsLong(n);

  tqdmciter->update = PyObject_GetAttrString(tqdm, "update");

  /* python tqdm.__iter__()
  if self.disable:
      for obj in iterable:
          yield obj
      return

  mininterval = self.mininterval
  last_print_t = self.last_print_t
  last_print_n = self.last_print_n
  min_start_t = self.start_t + self.delay
  n = self.n
  time = self._time

  try:
      for obj in iterable:
          yield obj
          # Update and possibly print the progressbar.
          # Note: does not call self.update(1) for speed optimisation.
          n += 1

          if n - last_print_n >= self.miniters:
              cur_t = time()
              dt = cur_t - last_print_t
              if dt >= mininterval and cur_t >= min_start_t:
                  self.update(n - last_print_n)
                  last_print_n = self.last_print_n
                  last_print_t = self.last_print_t
  finally:
      self.n = n
      self.close()
  */

  return (PyObject *)tqdmciter;
}

static void tqdmciter_dealloc(TqdmCIterState *tqdmciter) {
  /* We need XDECREF here because when the generator is exhausted,
   * rgstate->sequence is cleared with Py_CLEAR which sets it to NULL.
   */
  Py_XDECREF(tqdmciter->iter);
  Py_XDECREF(tqdmciter->tqdm);
  Py_TYPE(tqdmciter)->tp_free(tqdmciter);
}

static PyObject *tqdmciter_next(TqdmCIterState *tqdmciter) {
  PyObject *item = PyIter_Next(tqdmciter->iter);
  if (item == NULL) {
    // finally:
    PyObject *n = PyLong_FromLong(tqdmciter->n);
    PyObject_SetAttrString(tqdmciter->tqdm, "n", n);

    PyObject *close = PyObject_GetAttrString(tqdmciter->tqdm, "close");
    PyObject_CallNoArgs(close);
    return item;
  }

  tqdmciter->n++;
  // printf("tqdm.n = %ld\n", tqdmciter->n);

  PyObject *miniters = PyObject_GetAttrString(tqdmciter->tqdm, "miniters");
  if (!miniters) {
    printf("null miniters\n");
    return NULL;
  }
  long miniters_;
  if (PyLong_Check(miniters)) {
    miniters_ = PyLong_AsLong(miniters);
  } else if (PyFloat_Check(miniters)) {
    miniters_ = (long)PyFloat_AsDouble(miniters);
  } else {
    printf("bad miniters\n");
    return NULL;
  }

  long dn = tqdmciter->n - tqdmciter->last_print_n;
  if (dn >= miniters_) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double cur_t = ts.tv_sec + (ts.tv_nsec * 1e-9);
    if ((cur_t - tqdmciter->last_print_t) >= tqdmciter->mininterval &&
        cur_t >= tqdmciter->min_start_t) {

      // printf("Call: tqdm.update(%ld - %ld = %ld)\n", tqdmciter->n,
      // tqdmciter->last_print_n, dn);
      PyObject *args = Py_BuildValue("(i)", dn);
      PyObject_Call(tqdmciter->update, args, NULL);

      PyObject *last_print_t =
          PyObject_GetAttrString(tqdmciter->tqdm, "last_print_t");
      if (!last_print_t || !PyFloat_Check(last_print_t)) {

        printf("bad last_print_t\n");
        return NULL;
      }
      tqdmciter->last_print_t = PyFloat_AsDouble(last_print_t);
      // printf("tqdm.last_print_t = %f\n", tqdmciter->last_print_t);

      PyObject *last_print_n =
          PyObject_GetAttrString(tqdmciter->tqdm, "last_print_n");
      if (!last_print_n || !PyLong_Check(last_print_n))
        return NULL;
      tqdmciter->last_print_n = PyLong_AsLong(last_print_n);
      // printf("tqdm.last_print_n = %ld\n", tqdmciter->last_print_n);
    }
  }

  return item;
}

PyTypeObject PyTqdmCIter_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0) "tqdmciter", /* tp_name */
    sizeof(TqdmCIterState),                             /* tp_basicsize */
    0,                                                  /* tp_itemsize */
    (destructor)tqdmciter_dealloc,                      /* tp_dealloc */
    0,                                                  /* tp_print */
    0,                                                  /* tp_getattr */
    0,                                                  /* tp_setattr */
    0,                                                  /* tp_reserved */
    0,                                                  /* tp_repr */
    0,                                                  /* tp_as_number */
    0,                                                  /* tp_as_sequence */
    0,                                                  /* tp_as_mapping */
    0,                                                  /* tp_hash */
    0,                                                  /* tp_call */
    0,                                                  /* tp_str */
    0,                                                  /* tp_getattro */
    0,                                                  /* tp_setattro */
    0,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                                 /* tp_flags */
    0,                                                  /* tp_doc */
    0,                                                  /* tp_traverse */
    0,                                                  /* tp_clear */
    0,                                                  /* tp_richcompare */
    0,                                                  /* tp_weaklistoffset */
    PyObject_SelfIter,                                  /* tp_iter */
    (iternextfunc)tqdmciter_next,                       /* tp_iternext */
    0,                                                  /* tp_methods */
    0,                                                  /* tp_members */
    0,                                                  /* tp_getset */
    0,                                                  /* tp_base */
    0,                                                  /* tp_dict */
    0,                                                  /* tp_descr_get */
    0,                                                  /* tp_descr_set */
    0,                                                  /* tp_dictoffset */
    0,                                                  /* tp_init */
    PyType_GenericAlloc,                                /* tp_alloc */
    tqdmciter_new,                                      /* tp_new */
};

static PyMethodDef tqdmcMethods[] = {
    {"print", tqdmc_print, METH_VARARGS, "Print a string to stdout"},
    {"count", tqdmc_count, METH_VARARGS,
     "Iterate an iterable and return count"},
    {NULL, NULL, 0, NULL} /* Sentinel */
};

static struct PyModuleDef tqdmcmodule = {
    PyModuleDef_HEAD_INIT, "tqdmc", /* name of module */
    NULL,                           /* module documentation, may be NULL */
    -1, /* size of per-interpreter state of the module,
           or -1 if the module keeps state in global variables. */
    tqdmcMethods};

PyMODINIT_FUNC PyInit_tqdmc(void) {
  PyObject *m;

  m = PyModule_Create(&tqdmcmodule);
  if (m == NULL)
    return NULL;

  if (PyType_Ready(&PyYielder_Type) < 0)
    return NULL;
  Py_INCREF((PyObject *)&PyYielder_Type);
  PyModule_AddObject(m, "yielder", (PyObject *)&PyYielder_Type);

  if (PyType_Ready(&PyTqdmCIter_Type) < 0)
    return NULL;
  Py_INCREF((PyObject *)&PyTqdmCIter_Type);
  PyModule_AddObject(m, "tqdmciter", (PyObject *)&PyTqdmCIter_Type);

  return m;
}
