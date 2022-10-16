#include <Python.h>

static PyObject * tqdmc_print(PyObject *self, PyObject *args) {
    const char *command;
    int sts;

    if (!PyArg_ParseTuple(args, "s", &command))
        return NULL;
    write(1, command, strlen(command));
    return PyLong_FromLong(0);
}

static PyMethodDef tqdmcMethods[] = {
    {"print",  tqdmc_print, METH_VARARGS,
     "Print a string to stdout"},
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

