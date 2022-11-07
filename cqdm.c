#include <Python.h>

typedef struct {
  PyObject_HEAD Py_ssize_t seq_index, enum_index;
  PyObject *tqdm_obj;
  PyObject *iterator;
  PyObject *update_func;
  double mininterval;
  double last_print_t;
  long last_print_n;
  double min_start_t;
  long miniters;
  long n;
} CqdmState;

static PyObject *cqdm_new(PyTypeObject *type, PyObject *args,
                          PyObject *kwargs) {
  PyObject *tqdm_obj;

  if (!PyArg_UnpackTuple(args, "cqdm", 1, 1, &tqdm_obj)) {
    return NULL;
  }

  CqdmState *cqdm = (CqdmState *)type->tp_alloc(type, 0);
  if (!cqdm) {
    return NULL;
  }

  PyObject *iterable = PyObject_GetAttrString(tqdm_obj, "iterable");
  if (!iterable) {
    return NULL;
  }

  PyObject *iterator = PyObject_GetIter(iterable);
  if (!iterator) {
    return NULL;
  }

  PyObject *mininterval = PyObject_GetAttrString(tqdm_obj, "mininterval");
  if (!mininterval || !PyFloat_Check(mininterval)) {
    return NULL;
  }
  cqdm->mininterval = PyFloat_AsDouble(mininterval);

  PyObject *last_print_t = PyObject_GetAttrString(tqdm_obj, "last_print_t");
  if (!last_print_t || !PyFloat_Check(last_print_t)) {
    return NULL;
  }
  cqdm->last_print_t = PyFloat_AsDouble(last_print_t);

  PyObject *last_print_n = PyObject_GetAttrString(tqdm_obj, "last_print_n");
  if (!last_print_n || !PyLong_Check(last_print_n)) {
    return NULL;
  }
  cqdm->last_print_n = PyLong_AsLong(last_print_n);

  PyObject *start_t = PyObject_GetAttrString(tqdm_obj, "start_t");
  if (!start_t || !PyFloat_Check(start_t)) {
    return NULL;
  }
  PyObject *delay = PyObject_GetAttrString(tqdm_obj, "delay");
  if (!delay) {
    return NULL;
  }
  cqdm->min_start_t = PyFloat_AsDouble(PyNumber_Add(start_t, delay));

  PyObject *n = PyObject_GetAttrString(tqdm_obj, "n");
  if (!n || !PyLong_Check(n)) {
    return NULL;
  }
  cqdm->n = PyLong_AsLong(n);

  PyObject *miniters = PyObject_GetAttrString(tqdm_obj, "miniters");
  if (!miniters) {
    return NULL;
  }
  if (PyLong_Check(miniters)) {
    cqdm->miniters = PyLong_AsLong(miniters);
  } else if (PyFloat_Check(miniters)) {
    cqdm->miniters = (long)PyFloat_AsDouble(miniters);
  } else {
    return NULL;
  }

  PyObject *update_func = PyObject_GetAttrString(tqdm_obj, "update");
  if (!update_func) {
    return NULL;
  }

  // If we get through all those checks, things are looking good.
  // Let's INCREF the objects, and save them.
  Py_INCREF(tqdm_obj);
  cqdm->tqdm_obj = tqdm_obj;

  Py_INCREF(iterator);
  cqdm->iterator = iterator;

  Py_INCREF(update_func);
  cqdm->update_func = update_func;

  return (PyObject *)cqdm;

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
}

static void cqdm_dealloc(CqdmState *cqdm) {
  Py_XDECREF(cqdm->tqdm_obj);
  Py_XDECREF(cqdm->iterator);
  Py_XDECREF(cqdm->update_func);
  Py_TYPE(cqdm)->tp_free(cqdm);
}

static PyObject *cqdm_next(CqdmState *cqdm) {
  PyObject *item = PyIter_Next(cqdm->iterator);
  if (item == NULL) {
    // done iterating; run finally:
    PyObject *n = PyLong_FromLong(cqdm->n);
    PyObject_SetAttrString(cqdm->tqdm_obj, "n", n);
    Py_XDECREF(n);

    PyObject *close = PyObject_GetAttrString(cqdm->tqdm_obj, "close");
    PyObject_CallNoArgs(close);
    // close is a borrowed reference; no need to DECREF
    return item;
  }

  cqdm->n++;

  long dn = cqdm->n - cqdm->last_print_n;
  // We don't need to check miniters attribute on the object.
  // It was set because we overwrote __setattr__ to call cqdm_set_miniters().
  if (dn >= cqdm->miniters) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double cur_t = ts.tv_sec + (ts.tv_nsec * 1e-9);
    double dt = cur_t - cqdm->last_print_t;
    if (dt >= cqdm->mininterval && cur_t >= cqdm->min_start_t) {
      // self.update(n - last_print_n)
      PyObject *args = Py_BuildValue("(i)", dn);
      PyObject_Call(cqdm->update_func, args, NULL);
      Py_XDECREF(args);

      PyObject *last_print_t =
          PyObject_GetAttrString(cqdm->tqdm_obj, "last_print_t");
      if (!last_print_t || !PyFloat_Check(last_print_t)) {
        return NULL;
      }
      cqdm->last_print_t = PyFloat_AsDouble(last_print_t);

      PyObject *last_print_n =
          PyObject_GetAttrString(cqdm->tqdm_obj, "last_print_n");
      if (!last_print_n || !PyLong_Check(last_print_n)) {
        return NULL;
      }
      cqdm->last_print_n = PyLong_AsLong(last_print_n);
    }
  }

  return item;
}

static PyObject *cqdm_set_miniters(CqdmState *cqdm, PyObject *miniters) {
  if(!miniters) {
    return NULL;
  }
  if (PyLong_Check(miniters)) {
    cqdm->miniters = PyLong_AsLong(miniters);
  } else if (PyFloat_Check(miniters)) {
    cqdm->miniters = (long)PyFloat_AsDouble(miniters);
  } else {
    return NULL;
  }
  Py_RETURN_NONE;
}
static PyMethodDef cqdm_methods[] = {
    {"set_miniters", (PyCFunction) cqdm_set_miniters, METH_O,
     "Set miniters (used by monitor thread)" },
    {NULL}  /* Sentinel */
};

PyTypeObject PyCqdm_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    .tp_name = "cqdm",
    .tp_basicsize = sizeof(CqdmState),
    .tp_dealloc = (destructor)cqdm_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_iter = PyObject_SelfIter,
    .tp_iternext = (iternextfunc)cqdm_next,
    .tp_methods = cqdm_methods,
    .tp_alloc = PyType_GenericAlloc,
    .tp_new = cqdm_new,
};

static struct PyModuleDef cqdmmodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "cqdm_native",
    .m_size = -1,
};

PyMODINIT_FUNC PyInit_cqdm_native(void) {
  PyObject *m;

  m = PyModule_Create(&cqdmmodule);
  if (m == NULL)
    return NULL;

  if (PyType_Ready(&PyCqdm_Type) < 0)
    return NULL;
  Py_INCREF((PyObject *)&PyCqdm_Type);
  PyModule_AddObject(m, "cqdm", (PyObject *)&PyCqdm_Type);

  return m;
}
