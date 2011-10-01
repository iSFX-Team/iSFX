#include <Python.h>
#include "structmember.h"

#include "pySystem.hpp"
#include "pySound.hpp"

static PyModuleDef iSFXmodule = {
    PyModuleDef_HEAD_INIT,
    "iSFX",
    "A C++ and Python sound effects library wrapping fmod with other features.",
    -1,
    NULL, NULL, NULL, NULL, NULL
};

PyMODINIT_FUNC
PyInit_iSFX(void) 
{
    PyObject* m;

    if (PyType_Ready(&SystemType) < 0)
        return NULL;
    if (PyType_Ready(&SoundType) < 0)
        return NULL;

    m = PyModule_Create(&iSFXmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&SystemType);
    PyModule_AddObject(m, "System", (PyObject *)&SystemType);
    Py_INCREF(&SoundType);
    PyModule_AddObject(m, "Sound", (PyObject *)&SoundType);
    return m;
}