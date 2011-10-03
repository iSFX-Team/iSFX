#include <Python.h>
//#include "structmember.h"
#include <boost/bind.hpp>

// iSFX C++ Header
#include "iSFX.hpp"

// C Python Headers
#include "pySystem.hpp"

// Standard Headers
#include <iostream>

struct Sound {
    PyObject_HEAD
    iSFX::Sound* _sound;
    PyObject* atPositionCallback;
    PyObject* atPercentCallback;
    PyObject* inStateCallback;
    PyObject* starvingCallback;
    PyObject* newNameCallback;
    PyObject* newLengthMsCallback;
    PyObject* enteredCallback;
    PyObject* playingCallback;
    PyObject* pausedCallback;
    PyObject* exitedCallback;
    void pySocket_AtPosition(long i) {
      if (atPositionCallback != NULL) {
        PyObject *arglist = Py_BuildValue("(i)", i);
        PyObject_CallObject(atPositionCallback, arglist);
      }
    }
    void pySocket_AtPercent(double i) {
      if (atPercentCallback != NULL) {
        PyObject *arglist = Py_BuildValue("(d)", i);
        PyObject_CallObject(atPercentCallback, arglist);
      }
    }
    void pySocket_InState(unsigned int i) {
      if (inStateCallback != NULL) {
        PyObject *arglist = Py_BuildValue("(I)", i);
        PyObject_CallObject(inStateCallback, arglist);
      }
    }
    void pySocket_Starving(bool i) {
      if (starvingCallback != NULL) {
        PyObject *arglist = Py_BuildValue("(I)", i);
        PyObject_CallObject(starvingCallback, arglist);
      }
    }
    void pySocket_NewLengthMs(unsigned int i) {
      if (newLengthMsCallback != NULL) {
        PyObject *arglist = Py_BuildValue("(I)", i);
        PyObject_CallObject(newLengthMsCallback, arglist);
      }
    }
    void pySocket_NewName(std::string s) {
      if (newNameCallback != NULL) {
        PyObject *arglist = Py_BuildValue("(s)", s.c_str());
        PyObject_CallObject(newNameCallback, arglist);
      }
    }
    void pySocket_Entered(void) {
      if (enteredCallback != NULL) {
        PyObject *arglist = Py_BuildValue("");
        PyObject_CallObject(enteredCallback, arglist);
      }
    }
    void pySocket_Playing(bool b) {
      if (playingCallback != NULL) {
        PyObject *arglist = Py_BuildValue("(?)", b);
        PyObject_CallObject(playingCallback, arglist);
      }
    }
    void pySocket_Paused(bool b) {
      if (pausedCallback != NULL) {
        PyObject *arglist = Py_BuildValue("(?)", b);
        PyObject_CallObject(pausedCallback, arglist);
      }
    }
    void pySocket_Exited(void) {
      if (exitedCallback != NULL) {
        PyObject *arglist = Py_BuildValue("");
        PyObject_CallObject(exitedCallback, arglist);
      }
    }
};

static void
Sound_dealloc(Sound* self)
{
  delete self->_sound;
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject *
Sound_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Sound *self;

    self = (Sound *)type->tp_alloc(type, 0);
    if (self != NULL) {
      self->_sound = NULL;
      //self->update = boost::signals2::signal<void(unsigned int)>();
    }

    return (PyObject *)self;
}

static int
Sound_init(Sound *self, PyObject *args, PyObject *kwds)
{
  const char *sound_path;
  System* system;
  FMOD_RESULT res;
  if (!PyArg_ParseTuple(args, "Os", &system, &sound_path))
      return NULL;
  self->_sound = new iSFX::Sound(system->_system, sound_path);
  self->_sound->sigPosition.connect(boost::bind(&Sound::pySocket_AtPosition, self, _1));
  self->_sound->sigPercent.connect(boost::bind(&Sound::pySocket_AtPercent, self, _1));
  self->_sound->sigLengthMs.connect(boost::bind(&Sound::pySocket_NewLengthMs, self, _1));
  self->_sound->sigName.connect(boost::bind(&Sound::pySocket_NewName, self, _1));
  self->_sound->sigState.connect(boost::bind(&Sound::pySocket_InState, self, _1));
  self->_sound->sigStarving.connect(boost::bind(&Sound::pySocket_Starving, self, _1));
  
  self->_sound->sigEntered.connect(boost::bind(&Sound::pySocket_Entered, self));
  self->_sound->sigPlaying.connect(boost::bind(&Sound::pySocket_Playing, self, _1));
  self->_sound->sigPaused.connect(boost::bind(&Sound::pySocket_Paused, self, _1));
  self->_sound->sigExited.connect(boost::bind(&Sound::pySocket_Exited, self));
  //self->_sound->sigStopped.connect(boost::bind(&Sound::pySocket_NowStopped, self));
}


static PyMemberDef Sound_members[] = {
    {NULL}  /* Sentinel */
};

static PyObject * Sound_Play(Sound* self) { self->_sound->Play(); Py_RETURN_NONE; }
static PyObject * Sound_Pause(Sound* self) { self->_sound->Pause(); Py_RETURN_NONE; }
static PyObject * Sound_Stop(Sound* self) { self->_sound->Stop(); Py_RETURN_NONE; }
static PyObject * Sound_GetName(Sound* self) { return PyUnicode_FromString(self->_sound->GetName().c_str()); }
static PyObject * Sound_MemoryUsed(Sound* self) { return PyLong_FromLong(self->_sound->MemoryUsed()); }
static PyObject * Sound_PlayPause(Sound* self) { self->_sound->PlayPause(); Py_RETURN_NONE; }
static PyObject * Sound_PlayStop(Sound* self) { self->_sound->PlayStop(); Py_RETURN_NONE; }
static PyObject * Sound_GetPCMData(Sound* self) { self->_sound->GetPCMData(); Py_RETURN_NONE; }

static PyObject * Sound_SetPositionMs(Sound* self, PyObject *args, PyObject *keywds) {
  long ms;
  if (!PyArg_ParseTuple(args, "l", &ms)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed line: " + __LINE__);
    return NULL;
  }
  self->_sound->SkipTo(ms);
  Py_RETURN_NONE;
}

static PyObject * Sound_FadeOnStop(Sound* self, PyObject *args, PyObject *keywds) {
  long ms;
  if (!PyArg_ParseTuple(args, "l", &ms)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed line: " + __LINE__);
    return NULL;
  }
  self->_sound->SetFadeOnStop(ms);
  Py_RETURN_NONE;
}

static PyObject * Sound_FadeOnResume(Sound* self, PyObject *args, PyObject *keywds) {
  long ms;
  if (!PyArg_ParseTuple(args, "l", &ms)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed line: " + __LINE__);
    return NULL;
  }
  self->_sound->SetFadeOnResume(ms);
  Py_RETURN_NONE;
}

static PyObject * Sound_FadeOnPause(Sound* self, PyObject *args, PyObject *keywds) {
  long ms;
  if (!PyArg_ParseTuple(args, "l", &ms)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed line: " + __LINE__);
    return NULL;
  }
  self->_sound->SetFadeOnPause(ms);
  Py_RETURN_NONE;
}

static PyObject * Sound_FadeOnEnter(Sound* self, PyObject *args, PyObject *keywds) {
  long ms;
  if (!PyArg_ParseTuple(args, "l", &ms)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed line: " + __LINE__);
    return NULL;
  }
  self->_sound->SetFadeOnEnter(ms);
  Py_RETURN_NONE;
}

static PyObject * Sound_FadeOnExit(Sound* self, PyObject *args, PyObject *keywds) {
  long ms;
  if (!PyArg_ParseTuple(args, "l", &ms)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed line: " + __LINE__);
    return NULL;
  }
  self->_sound->SetFadeOnExit(ms);
  Py_RETURN_NONE;
}

static PyObject * Sound_FadeOnPlay(Sound* self, PyObject *args, PyObject *keywds) {
  long ms;
  if (!PyArg_ParseTuple(args, "l", &ms)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed line: " + __LINE__);
    return NULL;
  }
  self->_sound->SetFadeOnPlay(ms);
  Py_RETURN_NONE;
}

static PyObject * Sound_SetPositionCallback(Sound* self, PyObject* args) {
  PyObject* fun;
  if (!PyArg_ParseTuple(args, "O", &fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed.");
    return NULL;
  }
  if (!PyCallable_Check(fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
    return NULL;
  }
  
  Py_XINCREF(fun);         /* Add a reference to new callback */
  self->atPositionCallback = fun;
  
  Py_RETURN_NONE;
}

static PyObject * Sound_SetPercentCallback(Sound* self, PyObject* args) {
  PyObject* fun;
  if (!PyArg_ParseTuple(args, "O", &fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed.");
    return NULL;
  }
  if (!PyCallable_Check(fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
    return NULL;
  }
  
  Py_XINCREF(fun);         /* Add a reference to new callback */
  self->atPercentCallback = fun;
  
  Py_RETURN_NONE;
}

static PyObject * Sound_SetInStateCallback(Sound* self, PyObject* args) {
  PyObject* fun;
  if (!PyArg_ParseTuple(args, "O", &fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed.");
    return NULL;
  }
  if (!PyCallable_Check(fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
    return NULL;
  }
  
  Py_XINCREF(fun);         /* Add a reference to new callback */
  self->inStateCallback = fun;
  
  Py_RETURN_NONE;
}

static PyObject * Sound_SetStarvingCallback(Sound* self, PyObject* args) {
  PyObject* fun;
  if (!PyArg_ParseTuple(args, "O", &fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed.");
    return NULL;
  }
  if (!PyCallable_Check(fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
    return NULL;
  }
  
  Py_XINCREF(fun);         /* Add a reference to new callback */
  self->starvingCallback = fun;
  
  Py_RETURN_NONE;
}

static PyObject * Sound_SetNewLengthMsCallback(Sound* self, PyObject* args) {
  PyObject* fun;
  if (!PyArg_ParseTuple(args, "O", &fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed.");
    return NULL;
  }
  if (!PyCallable_Check(fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
    return NULL;
  }
  
  Py_XINCREF(fun);         /* Add a reference to new callback */
  self->newLengthMsCallback = fun;
  
  Py_RETURN_NONE;
}

static PyObject * Sound_SetNewNameCallback(Sound* self, PyObject* args) {
  PyObject* fun;
  if (!PyArg_ParseTuple(args, "O", &fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed.");
    return NULL;
  }
  if (!PyCallable_Check(fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
    return NULL;
  }
  
  Py_XINCREF(fun);         /* Add a reference to new callback */
  self->newNameCallback = fun;
  
  Py_RETURN_NONE;
}

static PyObject * Sound_SetPlayingCallback(Sound* self, PyObject* args) {
  PyObject* fun;
  if (!PyArg_ParseTuple(args, "O", &fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter parse failed.");
    return NULL;
  }
  if (!PyCallable_Check(fun)) {
    PyErr_SetString(PyExc_TypeError, "parameter must be callable.");
    return NULL;
  }
  
  Py_XINCREF(fun);         /* Add a reference to new callback */
  self->playingCallback = fun;
  
  Py_RETURN_NONE;
}

static PyMethodDef Sound_methods[] = {
    {"play", (PyCFunction)Sound_Play, METH_NOARGS, "Plays the sound" }, 
    {"stop", (PyCFunction)Sound_Stop, METH_NOARGS, "Stops the sound" },
    {"pause", (PyCFunction)Sound_Pause, METH_NOARGS, "Pauses the sound" },
    {"getName", (PyCFunction)Sound_GetName, METH_NOARGS, "Returns sound's name." },
    {"memoryUsed", (PyCFunction)Sound_MemoryUsed, METH_NOARGS, "Returns the number of bytes used by the sound." },
    {"playStop", (PyCFunction)Sound_PlayStop, METH_NOARGS, "Toggles play/stop" },
    {"playPause", (PyCFunction)Sound_PlayPause, METH_NOARGS, "Toggles play/pause" },
    
    {"fadeOnPlay", (PyCFunction)Sound_FadeOnPlay, METH_VARARGS, "Sets the fade-on-play option." },
    {"fadeOnStop", (PyCFunction)Sound_FadeOnStop, METH_VARARGS, "Sets the fade-on-stop option." },
    {"fadeOnPause", (PyCFunction)Sound_FadeOnPause, METH_VARARGS, "Sets the fade-on-pause option." },
    {"fadeOnResume", (PyCFunction)Sound_FadeOnResume, METH_VARARGS, "Sets the fade-on-resume option." },
    {"fadeEntering", (PyCFunction)Sound_FadeOnEnter, METH_VARARGS, "Fade on sound beginning." },
    {"fadeExiting", (PyCFunction)Sound_FadeOnExit, METH_VARARGS, "Fade on sound finish." },
    
    {"setPositionCallback", (PyCFunction)Sound_SetPositionCallback, METH_VARARGS, "Sets the change in position callback." },
    {"setPercentCallback", (PyCFunction)Sound_SetPercentCallback, METH_VARARGS, "Sets the change in position percentage callback." },
    {"setInStateCallback", (PyCFunction)Sound_SetInStateCallback, METH_VARARGS, "Sets the state change callback." },
    {"setStarvingCallback", (PyCFunction)Sound_SetStarvingCallback, METH_VARARGS, "Sets the starving callback." },
    {"setNewNameCallback", (PyCFunction)Sound_SetNewNameCallback, METH_VARARGS, "Sets new name callback." },
    {"setNewLengthMsCallback", (PyCFunction)Sound_SetNewLengthMsCallback, METH_VARARGS, "Sets new length callback." },
    {"setPlayingCallback", (PyCFunction)Sound_SetPlayingCallback, METH_VARARGS, "Sets playing callback (0 if stopped)." },
    {"setPositionMs", (PyCFunction)Sound_SetPositionMs, METH_VARARGS, "Changes the current position (ms)." },
    {NULL}  /* Sentinel */
};

static PyTypeObject SoundType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "iSFX.Sound",             /* tp_name */
    sizeof(Sound),             /* tp_basicsize */
    0,                         /* tp_itemsize */
    (destructor)Sound_dealloc, /* tp_dealloc */
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
    "Sound objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Sound_methods,             /* tp_methods */
    Sound_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)Sound_init,      /* tp_init */
    0,                         /* tp_alloc */
    Sound_new,                 /* tp_new */
};