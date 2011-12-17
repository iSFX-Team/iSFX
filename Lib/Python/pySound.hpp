#include <Python.h>
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
};

static void
Sound_dealloc(Sound* self) {
  delete self->_sound;
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject*
Sound_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    Sound *self;
    self = (Sound *)type->tp_alloc(type, 0);
    if (self != NULL) {
      self->_sound = NULL;
    }
    return (PyObject *)self;
}

static int
Sound_init(Sound *self, PyObject *args, PyObject *kwds) {
  const char *sound_path;
  System* system;
  uint32_t length = 0;
  const char *waveform_path;
  FMOD_RESULT res;
  if (!PyArg_ParseTuple(args, "OsIs", &system, &sound_path, &length, &waveform_path))
      return NULL;
  self->_sound = new iSFX::Sound(system->_system, sound_path, length, waveform_path);
  return 0;
}

static PyObject * Sound_setMasterVolume(Sound* self, PyObject* args) {
  float a;
  if (!PyArg_ParseTuple(args, "f", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setMasterVolume"); return NULL; }
  self->_sound->setMasterVolume(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setMaxVolume(Sound* self, PyObject* args) {
  float a;
  if (!PyArg_ParseTuple(args, "f", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setMaxVolume"); return NULL; }
  self->_sound->setMaxVolume(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setMinVolume(Sound* self, PyObject* args) {
  float a;
  if (!PyArg_ParseTuple(args, "f", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setMinVolume"); return NULL; }
  self->_sound->setMinVolume(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setPosition(Sound* self, PyObject* args) {
  uint32_t a;
  if (!PyArg_ParseTuple(args, "I", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setPosition"); return NULL; }
  self->_sound->setPosition(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setStart(Sound* self, PyObject* args) {
  uint32_t a;
  if (!PyArg_ParseTuple(args, "I", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setStart"); return NULL; }
  self->_sound->setStart(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setStop(Sound* self, PyObject* args) {
  long a;
  if (!PyArg_ParseTuple(args, "l", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setStop"); return NULL; }
  self->_sound->setStop(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setFadeIn(Sound* self, PyObject* args) {
  uint32_t a;
  if (!PyArg_ParseTuple(args, "I", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setFadeIn"); return NULL; }
  self->_sound->setFadeIn(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setFadeOut(Sound* self, PyObject* args) {
  uint32_t a;
  if (!PyArg_ParseTuple(args, "I", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setFadeOut"); return NULL; }
  self->_sound->setFadeOut(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setFadeStop(Sound* self, PyObject* args) {
  uint32_t a;
  if (!PyArg_ParseTuple(args, "I", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setFadeStop"); return NULL; }
  self->_sound->setFadeStop(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setLowPassFilter(Sound* self, PyObject* args) {
  uint32_t a;
  if (!PyArg_ParseTuple(args, "I", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setLowPassFilter"); return NULL; }
  self->_sound->setLowPassFilter(a);
  Py_RETURN_NONE;
}
static PyObject * Sound_setHighPassFilter(Sound* self, PyObject* args) {
  uint32_t a;
  if (!PyArg_ParseTuple(args, "I", &a)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_setHighPassFilter"); return NULL; }
  self->_sound->setHighPassFilter(a);
  Py_RETURN_NONE;
}

static PyObject * Sound_play(Sound* self) { self->_sound->play(); Py_RETURN_NONE; }
static PyObject * Sound_load(Sound* self) { self->_sound->load(); Py_RETURN_NONE; }
static PyObject * Sound_stream(Sound* self) { self->_sound->stream(); Py_RETURN_NONE; }
static PyObject * Sound_playStop(Sound* self) { self->_sound->playStop(); Py_RETURN_NONE; }
static PyObject * Sound_pause(Sound* self) { self->_sound->pause(); Py_RETURN_NONE; }
static PyObject * Sound_unpause(Sound* self) { self->_sound->unpause(); Py_RETURN_NONE; }
static PyObject * Sound_stop(Sound* self) { self->_sound->stop(); Py_RETURN_NONE; }
static PyObject * Sound_kill(Sound* self) { self->_sound->kill(); Py_RETURN_NONE; }
static PyObject * Sound_getWaveform(Sound* self, PyObject* args) {
  const char * s;
  if (!PyArg_ParseTuple(args, "s", &s)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed in Sound_getWaveform"); return NULL; }
  self->_sound->getWaveform(s);
  Py_RETURN_NONE;
}

PyObject * CallbackHelperS(PyObject* f, std::string n) {
  return PyObject_CallObject(f, Py_BuildValue("(s)", n.c_str()));
}

PyObject * CallbackHelperD(PyObject* f, double d) {
  return PyObject_CallObject(f, Py_BuildValue("(d)", d));
}

PyObject * CallbackHelperF(PyObject* f, float d) {
  return PyObject_CallObject(f, Py_BuildValue("(f)", d));
}

PyObject * CallbackHelperI(PyObject* f, uint32_t d) {
  return PyObject_CallObject(f, Py_BuildValue("(I)", d));
}

PyObject * CallbackHelperB(PyObject* f, uint8_t d) {
  return PyObject_CallObject(f, Py_BuildValue("(b)", d));
}

PyObject * ParseAndCheckCallable(PyObject* args) {
  PyObject* fun;
  if (!PyArg_ParseTuple(args, "O", &fun)) { PyErr_SetString(PyExc_TypeError, "parameter parse failed"); return NULL; }
  if (!PyCallable_Check(fun)) { PyErr_SetString(PyExc_TypeError, "parameter must be callable"); return NULL; }
  Py_XINCREF(fun);
  return fun;
}

static PyObject * Sound_onNameChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->nameChanged.connect(boost::bind(&CallbackHelperS, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onFilePathChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->filePathChanged.connect(boost::bind(&CallbackHelperS, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onLengthChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->lengthChanged.connect(boost::bind(&CallbackHelperI, fun, _1));
  Py_RETURN_NONE;
}

static PyObject * Sound_onStartChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->startChanged.connect(boost::bind(&CallbackHelperI, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onStopChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->stopChanged.connect(boost::bind(&CallbackHelperI, fun, _1));
  Py_RETURN_NONE;
}

static PyObject * Sound_onFadingInChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->fadingInChanged.connect(boost::bind(&CallbackHelperB, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onFadingOutChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->fadingOutChanged.connect(boost::bind(&CallbackHelperB, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onFadingStopChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->fadingStopChanged.connect(boost::bind(&CallbackHelperB, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onFadeInChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->fadeInChanged.connect(boost::bind(&CallbackHelperI, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onFadeOutChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->fadeOutChanged.connect(boost::bind(&CallbackHelperI, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onFadeStopChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->fadeStopChanged.connect(boost::bind(&CallbackHelperI, fun, _1));
  Py_RETURN_NONE;
}

static PyObject * Sound_onPositionChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->positionChanged.connect(boost::bind(&CallbackHelperI, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onPercentChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->percentChanged.connect(boost::bind(&CallbackHelperD, fun, _1));
  Py_RETURN_NONE;
}

static PyObject * Sound_onPlayingChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->playingChanged.connect(boost::bind(&CallbackHelperB, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onPausedChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->pausedChanged.connect(boost::bind(&CallbackHelperB, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onLoadingChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->loadingChanged.connect(boost::bind(&CallbackHelperB, fun, _1));
  Py_RETURN_NONE;
}

static PyObject * Sound_onMasterVolumeChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->masterVolumeChanged.connect(boost::bind(&CallbackHelperF, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onActualVolumeChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->actualVolumeChanged.connect(boost::bind(&CallbackHelperF, fun, _1));
  Py_RETURN_NONE;
}
static PyObject * Sound_onWaveformFileChanged(Sound* self, PyObject* args) {
  PyObject* fun = ParseAndCheckCallable(args);
  self->_sound->waveformFileChanged.connect(boost::bind(&CallbackHelperS, fun, _1));
  Py_RETURN_NONE;
}

static PyMemberDef Sound_members[] = {{NULL}};
static PyMethodDef Sound_methods[] = {
  
  {"load", (PyCFunction)Sound_load, METH_VARARGS, "" },
  {"stream", (PyCFunction)Sound_stream, METH_VARARGS, "" },
  {"play", (PyCFunction)Sound_play, METH_VARARGS, "" },
  {"playStop", (PyCFunction)Sound_playStop, METH_VARARGS, "" },
  {"pause", (PyCFunction)Sound_pause, METH_VARARGS, "" },
  {"unpause", (PyCFunction)Sound_unpause, METH_VARARGS, "" },
  {"stop", (PyCFunction)Sound_stop, METH_VARARGS, "" },
  {"kill", (PyCFunction)Sound_kill, METH_VARARGS, "" },
  {"getWaveform", (PyCFunction)Sound_getWaveform, METH_VARARGS, "" },
  
  {"setMasterVolume", (PyCFunction)Sound_setMasterVolume, METH_VARARGS, "" },
  {"setMaxVolume", (PyCFunction)Sound_setMaxVolume, METH_VARARGS, "" },
  {"setMinVolume", (PyCFunction)Sound_setMinVolume, METH_VARARGS, "" },
  {"setPosition", (PyCFunction)Sound_setPosition, METH_VARARGS, "" },
  {"setStart", (PyCFunction)Sound_setStart, METH_VARARGS, "" },
  {"setStop", (PyCFunction)Sound_setStop, METH_VARARGS, "" },
  {"setFadeIn", (PyCFunction)Sound_setFadeIn, METH_VARARGS, "" },
  {"setFadeOut", (PyCFunction)Sound_setFadeOut, METH_VARARGS, "" },
  {"setFadeStop", (PyCFunction)Sound_setFadeStop, METH_VARARGS, "" },
  {"setLowPassFilter", (PyCFunction)Sound_setLowPassFilter, METH_VARARGS, "" },
  {"setHighPassFilter", (PyCFunction)Sound_setHighPassFilter, METH_VARARGS, "" },
  
  
  {"onNameChanged", (PyCFunction)Sound_onNameChanged, METH_VARARGS, "" },
  {"onFilePathChanged", (PyCFunction)Sound_onFilePathChanged, METH_VARARGS, "" },
  {"onLengthChanged", (PyCFunction)Sound_onLengthChanged, METH_VARARGS, "" },
  {"onStartChanged", (PyCFunction)Sound_onStartChanged, METH_VARARGS, "" },
  {"onStopChanged", (PyCFunction)Sound_onStopChanged, METH_VARARGS, "" },
  {"onFadingInChanged", (PyCFunction)Sound_onFadingInChanged, METH_VARARGS, "" },
  {"onFadingOutChanged", (PyCFunction)Sound_onFadingOutChanged, METH_VARARGS, "" },
  {"onFadingStopChanged", (PyCFunction)Sound_onFadingStopChanged, METH_VARARGS, "" },
  {"onFadeInChanged", (PyCFunction)Sound_onFadeInChanged, METH_VARARGS, "" },
  {"onFadeOutChanged", (PyCFunction)Sound_onFadeOutChanged, METH_VARARGS, "" },
  {"onFadeStopChanged", (PyCFunction)Sound_onFadeStopChanged, METH_VARARGS, "" },
  {"onPositionChanged", (PyCFunction)Sound_onPositionChanged, METH_VARARGS, "" },
  {"onPercentChanged", (PyCFunction)Sound_onPercentChanged, METH_VARARGS, "" },
  {"onPlayingChanged", (PyCFunction)Sound_onPlayingChanged, METH_VARARGS, "" },
  {"onPausedChanged", (PyCFunction)Sound_onPausedChanged, METH_VARARGS, "" },
  {"onLoadingChanged", (PyCFunction)Sound_onLoadingChanged, METH_VARARGS, "" },
  {"onMasterVolumeChanged", (PyCFunction)Sound_onMasterVolumeChanged, METH_VARARGS, "" },
  {"onActualVolumeChanged", (PyCFunction)Sound_onActualVolumeChanged, METH_VARARGS, "" },
  {"onWaveformFileChanged", (PyCFunction)Sound_onWaveformFileChanged, METH_VARARGS, "" },
  {NULL}  /* Sentinel */
};

static PyTypeObject SoundType = {
                     PyVarObject_HEAD_INIT(NULL, 0)
                              "iSFX.Sound", /* tp_name */
                             sizeof(Sound), /* tp_basicsize */
                                         0, /* tp_itemsize */
                 (destructor)Sound_dealloc, /* tp_dealloc */
                                         0, /* tp_print */
                                         0, /* tp_getattr */
                                         0, /* tp_setattr */
                                         0, /* tp_reserved */
                                         0, /* tp_repr */
                                         0, /* tp_as_number */
                                         0, /* tp_as_sequence */
                                         0, /* tp_as_mapping */
                                         0, /* tp_hash */
                                         0, /* tp_call */
                                         0, /* tp_str */
                                         0, /* tp_getattro */
                                         0, /* tp_setattro */
                                         0, /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
                           "Sound objects", /* tp_doc */
                                         0, /* tp_traverse */
                                         0, /* tp_clear */
                                         0, /* tp_richcompare */
                                         0, /* tp_weaklistoffset */
                                         0, /* tp_iter */
                                         0, /* tp_iternext */
                             Sound_methods, /* tp_methods */
                             Sound_members, /* tp_members */
                                         0, /* tp_getset */
                                         0, /* tp_base */
                                         0, /* tp_dict */
                                         0, /* tp_descr_get */
                                         0, /* tp_descr_set */
                                         0, /* tp_dictoffset */
                      (initproc)Sound_init, /* tp_init */
                                         0, /* tp_alloc */
                                 Sound_new, /* tp_new */
};