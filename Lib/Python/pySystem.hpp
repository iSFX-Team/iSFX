#ifndef SYSTEM_HPP_OLSKSV6G
#define SYSTEM_HPP_OLSKSV6G

#include <Python.h>
#include "structmember.h"

#include "iSFX.hpp"
#include <iostream>
#include <sstream>
#include <string>

struct System {
    PyObject_HEAD
    iSFX::System* _system;
};

static void
System_dealloc(System* self)
{
  delete self->_system;
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
System_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    System *self;

    self = (System *)type->tp_alloc(type, 0);
    if (self != NULL) {
      self->_system = NULL;
    }

    return (PyObject *)self;
}

static int
System_init(System *self, PyObject *args, PyObject *kwds)
{
  std::cout << "System_init" << std::flush;
  self->_system = new iSFX::System();
  std::cout << " success" << std::endl << std::flush;
  return 0;
}

static PyObject*
System_Update(System *self)
{
  self->_system->update();
  Py_RETURN_NONE;
}

static PyObject*
System_getWaveData(System *self, PyObject* args) {
  uint8_t u;
  if (!PyArg_ParseTuple(args, "I", &u)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in System_getWaveData"); return NULL; }
  const std::vector<float>& data = self->_system->getWaveData(u);
  std::stringstream ss;
  ss.precision(4);
  for (int i = 0; i < data.size(); i++) {
    ss << i << "," << data[i] << "\n";
  }
  ss.flush();
  PyObject* ret = Py_BuildValue("s", ss.str().c_str());
  if (ret == NULL) std::cout << "RET == NULL" << std::endl;
  //Py_XINCREF(ret);
  return ret;
}


static PyMemberDef System_members[] = {
    {NULL}  /* Sentinel */
};

static PyMethodDef System_methods[] = {
    {"Update", (PyCFunction)System_Update, METH_NOARGS, "Updates the system" },
    {"getWaveData", (PyCFunction)System_getWaveData, METH_VARARGS, "" },
    {NULL}  /* Sentinel */
};

static PyTypeObject SystemType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iSFX.System",             /* tp_name */
    sizeof(System),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)System_dealloc, /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "System objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    System_methods,             /* tp_methods */
    System_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)System_init,      /* tp_init */
    0,                         /* tp_alloc */
    System_new,                 /* tp_new */
};

#endif /* end of include guard: SYSTEM_HPP_OLSKSV6G */