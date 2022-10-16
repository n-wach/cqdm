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

    return m;
}

