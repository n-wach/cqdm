#include <Python.h>

static PyObject * tqdmc_print(PyObject *self, PyObject *args) {
    const char *command;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;

    write(1, command, strlen(command));

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * tqdmc_count(PyObject *self, PyObject *args) {
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
    PyObject_HEAD
    Py_ssize_t seq_index, enum_index;
    PyObject *iterable;
} YielderState;


static PyObject *
yielder_new(PyTypeObject *type, PyObject *args, PyObject *kwargs)
{
    PyObject *obj;

    if (!PyArg_UnpackTuple(args, "yielder", 1, 1, &obj))
        return NULL;

    YielderState *yielder = (YielderState *)type->tp_alloc(type, 0);
    if (!yielder)
        return NULL;

    PyObject *iterable = PyObject_GetIter(obj);
    Py_INCREF(iterable);
    yielder->iterable = iterable;

    return (PyObject *)yielder;
}


static void
yielder_dealloc(YielderState *yielder)
{
    /* We need XDECREF here because when the generator is exhausted,
     * rgstate->sequence is cleared with Py_CLEAR which sets it to NULL.
    */
    Py_XDECREF(yielder->iterable);
    Py_TYPE(yielder)->tp_free(yielder);
}


static PyObject *
yielder_next(YielderState *yielder)
{
    PyObject *item = PyIter_Next(yielder->iterable);
    return item;
}


PyTypeObject PyYielder_Type = {
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "yielder",                       /* tp_name */
    sizeof(YielderState),            /* tp_basicsize */
    0,                              /* tp_itemsize */
    (destructor)yielder_dealloc,     /* tp_dealloc */
    0,                              /* tp_print */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
    0,                              /* tp_reserved */
    0,                              /* tp_repr */
    0,                              /* tp_as_number */
    0,                              /* tp_as_sequence */
    0,                              /* tp_as_mapping */
    0,                              /* tp_hash */
    0,                              /* tp_call */
    0,                              /* tp_str */
    0,                              /* tp_getattro */
    0,                              /* tp_setattro */
    0,                              /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,             /* tp_flags */
    0,                              /* tp_doc */
    0,                              /* tp_traverse */
    0,                              /* tp_clear */
    0,                              /* tp_richcompare */
    0,                              /* tp_weaklistoffset */
    PyObject_SelfIter,              /* tp_iter */
    (iternextfunc)yielder_next,      /* tp_iternext */
    0,                              /* tp_methods */
    0,                              /* tp_members */
    0,                              /* tp_getset */
    0,                              /* tp_base */
    0,                              /* tp_dict */
    0,                              /* tp_descr_get */
    0,                              /* tp_descr_set */
    0,                              /* tp_dictoffset */
    0,                              /* tp_init */
    PyType_GenericAlloc,            /* tp_alloc */
    yielder_new,                     /* tp_new */
};


static PyMethodDef tqdmcMethods[] = {
    {"print",  tqdmc_print, METH_VARARGS,
     "Print a string to stdout"},
    {"count",  tqdmc_count, METH_VARARGS,
     "Iterate an iterable and return count"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef tqdmcmodule = {
   PyModuleDef_HEAD_INIT,
   "tqdmc",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   tqdmcMethods
};

PyMODINIT_FUNC PyInit_tqdmc(void)
{
    PyObject *m;

    m = PyModule_Create(&tqdmcmodule);
    if (m == NULL)
        return NULL;

    if (PyType_Ready(&PyYielder_Type) < 0)
        return NULL;

    Py_INCREF((PyObject *)&PyYielder_Type);
    PyModule_AddObject(m, "yielder", (PyObject *)&PyYielder_Type);

    return m;
}

