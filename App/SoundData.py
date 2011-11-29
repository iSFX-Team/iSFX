from PyQt4 import QtCore

"""
Match:
    ([^ ]*) (.*)

Replace:     
    # $1
    def set\\u$1(self, $1):
        if self._$1 != $1:
            self._$1 = $1
            self.$1Changed.emit()
            
    @QtCore.pyqtProperty($2, fset=set\\u$1, notify=$1Changed)
    def $1(self):
        return self._$1


            
Note: \\u makes next character uppercase (go TextMate)
"""

class SoundData(QtCore.QObject):
  
    # qml does not use the signals parameters
    # but C++ pretty much requires it, unless
    # you want to call a getter when it gets
    # a signal
    nameChanged = QtCore.pyqtSignal(str)
    filePathChanged = QtCore.pyqtSignal(str)
    indexChanged = QtCore.pyqtSignal(int)
    lengthChanged = QtCore.pyqtSignal(int)
    
    startChanged = QtCore.pyqtSignal(int)
    stopChanged = QtCore.pyqtSignal(int)
    
    fadingInChanged = QtCore.pyqtSignal(int)
    fadingOutChanged = QtCore.pyqtSignal(int)
    fadingStopChanged = QtCore.pyqtSignal(int)
    fadeInChanged = QtCore.pyqtSignal(int)
    fadeOutChanged = QtCore.pyqtSignal(int)
    fadeStopChanged = QtCore.pyqtSignal(int)
    
    positionChanged = QtCore.pyqtSignal(int)
    percentChanged = QtCore.pyqtSignal(float)
    
    playingChanged = QtCore.pyqtSignal(int)
    pausedChanged = QtCore.pyqtSignal(int)
    loadingChanged = QtCore.pyqtSignal(int)
    
    masterVolumeChanged = QtCore.pyqtSignal(float)
    actualVolumeChanged = QtCore.pyqtSignal(float)
    
    waveformFileChanged = QtCore.pyqtSignal(str)

    def __init__(self):
        super(SoundData, self).__init__()
        
        self._backend = None
        
        self._name = ""
        self._filePath = ""
        self._index = 0
        self._length = 0
        
        self._start = 0
        self._stop = 0
        
        self._fadingIn = False
        self._fadingOut = False
        self._fadingStop = False
        self._fadeIn = 1
        self._fadeOut = 1
        self._fadeStop = 1
        
        self._position = 0
        self._percent = 0
        
        self._playing = False
        self._paused = False
        self._loading = False
        
        self._masterVolume = 1.0
        self._actualVolume = 0.0
        
        self._waveformFile = ""
        
    def setBackend(self, snd):
      self._backend = snd
      #maxVolumeChanged.connect(self._backend.setMaxVolume)
      #minVolumeChanged.connect(self._backend.setMinVolume)
      #self.positionChanged.connect(self._backend.setPosition)
      self.startChanged.connect(self._backend.setStart)
      self.stopChanged.connect(self._backend.setStop)
      self.fadeInChanged.connect(self._backend.setFadeIn)
      self.fadeOutChanged.connect(self._backend.setFadeOut)
      self.fadeStopChanged.connect(self._backend.setFadeStop)
      self.masterVolumeChanged.connect(self._backend.setMasterVolume)
      
      if (self._length == 0):
        self._backend.onNameChanged(self.setName)
      self._backend.onFilePathChanged(self.setFilePath)
      #self._backend.onIndexChanged(self.setIndex)
      self._backend.onLengthChanged(self.setLength)
      self._backend.onStartChanged(self.setStart)
      self._backend.onStopChanged(self.setStop)
      self._backend.onFadingInChanged(self.setFadingIn)
      self._backend.onFadingOutChanged(self.setFadingOut)
      self._backend.onFadingStopChanged(self.setFadingStop)
      #self._backend.onFadeInChanged(self.setFadeIn)
      #self._backend.onFadeOutChanged(self.setFadeOut)
      #self._backend.onFadeStopChanged(self.setFadeStop)
      self._backend.onPositionChanged(self.setPosition)
      self._backend.onPercentChanged(self.setPercent)
      self._backend.onPlayingChanged(self.setPlaying)
      self._backend.onPausedChanged(self.setPaused)
      self._backend.onLoadingChanged(self.setLoading)
      #self._backend.onMasterVolumeChanged(self.setMasterVolume)
      self._backend.onActualVolumeChanged(self.setActualVolume)
      self._backend.onWaveformFileChanged(self.setWaveformFile)
      
      if (self._length != 0):
        self._backend.setStart(self._start)
        self._backend.setStop(self._stop)
        self._backend.setFadeIn(self._fadeIn)
        self._backend.setFadeOut(self._fadeOut)
        self._backend.setFadeStop(self._fadeStop)
        self._backend.setMasterVolume(self._masterVolume)
      
    
    @QtCore.pyqtSlot()
    def doPlay(self):
      if not (self._backend == None):
        self._backend.play()
        
    @QtCore.pyqtSlot()
    def doPlayStop(self):
      if not (self._backend == None):
        self._backend.playStop()
    
    @QtCore.pyqtSlot()
    def doPause(self):
      if not (self._backend == None):
        self._backend.pause()
    
    @QtCore.pyqtSlot()
    def doUnpause(self):
      if not (self._backend == None):
        self._backend.unpause()
    
    @QtCore.pyqtSlot()
    def doStop(self):
      if not (self._backend == None):
        self._backend.stop()
    
    #@QtCore.pyqtSlot()
    #def getWaveform(self):
    #  if not (self._backend == None):
    #    self._backend.getWaveform()

    # name
    def setName(self, name):
        if self._name != name:
            self._name = name
            self.nameChanged.emit(self._name)
            
    @QtCore.pyqtProperty(str, fset=setName, notify=nameChanged)
    def name(self):
        return self._name

    # filePath
    def setFilePath(self, filePath):
        if self._filePath != filePath:
            self._filePath = filePath
            self.filePathChanged.emit(self._filePath)
            
    @QtCore.pyqtProperty(str, fset=setFilePath, notify=filePathChanged)
    def filePath(self):
        return self._filePath

    # index
    def setIndex(self, index):
        if self._index != index:
            self._index = index
            self.indexChanged.emit(self._index)
            
    @QtCore.pyqtProperty(int, fset=setIndex, notify=indexChanged)
    def index(self):
        return self._index

    # length
    def setLength(self, length):
        if self._length != length:
            self._length = length
            self.lengthChanged.emit(self._length)
            
    @QtCore.pyqtProperty(int, fset=setLength, notify=lengthChanged)
    def length(self):
        return self._length

    # start
    def setStart(self, start):
        if self._start != start:
            self._start = start
            self.startChanged.emit(self._start)
            
    @QtCore.pyqtProperty(int, fset=setStart, notify=startChanged)
    def start(self):
        return self._start

    # stop
    def setStop(self, stop):
        if self._stop != stop:
            self._stop = stop
            self.stopChanged.emit(self._stop)
            
    @QtCore.pyqtProperty(int, fset=setStop, notify=stopChanged)
    def stop(self):
        return self._stop

    # fadingIn
    def setFadingIn(self, fadingIn):
        if self._fadingIn != fadingIn:
            self._fadingIn = fadingIn
            self.fadingInChanged.emit(self._fadingIn)
            
    @QtCore.pyqtProperty(int, fset=setFadingIn, notify=fadingInChanged)
    def fadingIn(self):
        return self._fadingIn

    # fadingOut
    def setFadingOut(self, fadingOut):
        if self._fadingOut != fadingOut:
            self._fadingOut = fadingOut
            self.fadingOutChanged.emit(self._fadingOut)
            
    @QtCore.pyqtProperty(int, fset=setFadingOut, notify=fadingOutChanged)
    def fadingOut(self):
        return self._fadingOut

    # fadingStop
    def setFadingStop(self, fadingStop):
        if self._fadingStop != fadingStop:
            self._fadingStop = fadingStop
            self.fadingStopChanged.emit(self._fadingStop)
            
    @QtCore.pyqtProperty(int, fset=setFadingStop, notify=fadingStopChanged)
    def fadingStop(self):
        return self._fadingStop

    # fadeIn
    def setFadeIn(self, fadeIn):
        if self._fadeIn != fadeIn:
            self._fadeIn = fadeIn
            self.fadeInChanged.emit(self._fadeIn)
            
    @QtCore.pyqtProperty(int, fset=setFadeIn, notify=fadeInChanged)
    def fadeIn(self):
        return self._fadeIn

    # fadeOut
    def setFadeOut(self, fadeOut):
        if self._fadeOut != fadeOut:
            self._fadeOut = fadeOut
            self.fadeOutChanged.emit(self._fadeOut)
            
    @QtCore.pyqtProperty(int, fset=setFadeOut, notify=fadeOutChanged)
    def fadeOut(self):
        return self._fadeOut

    # fadeStop
    def setFadeStop(self, fadeStop):
        if self._fadeStop != fadeStop:
            self._fadeStop = fadeStop
            self.fadeStopChanged.emit(self._fadeStop)
            
    @QtCore.pyqtProperty(int, fset=setFadeStop, notify=fadeStopChanged)
    def fadeStop(self):
        return self._fadeStop

    # position
    def setPosition(self, position):
        if self._position != position:
            self._position = position
            self.positionChanged.emit(self._position)
            
    @QtCore.pyqtProperty(int, fset=setPosition, notify=positionChanged)
    def position(self):
        return self._position

    # percent
    def setPercent(self, percent):
        if self._percent != percent:
            self._percent = percent
            self.percentChanged.emit(self._percent)
            
    @QtCore.pyqtProperty(float, fset=setPercent, notify=percentChanged)
    def percent(self):
        return self._percent

    # playing
    def setPlaying(self, playing):
        if self._playing != playing:
            self._playing = playing
            self.playingChanged.emit(self._playing)
            
    @QtCore.pyqtProperty(int, fset=setPlaying, notify=playingChanged)
    def playing(self):
        return self._playing

    # paused
    def setPaused(self, paused):
        if self._paused != paused:
            self._paused = paused
            self.pausedChanged.emit(self._paused)
            
    @QtCore.pyqtProperty(int, fset=setPaused, notify=pausedChanged)
    def paused(self):
        return self._paused

    # loading
    def setLoading(self, loading):
        if self._loading != loading:
            self._loading = loading
            self.loadingChanged.emit(self._loading)
            
    @QtCore.pyqtProperty(int, fset=setLoading, notify=loadingChanged)
    def loading(self):
        return self._loading

    # masterVolume
    def setMasterVolume(self, masterVolume):
        if self._masterVolume != masterVolume:
            self._masterVolume = masterVolume
            self.masterVolumeChanged.emit(self._masterVolume)
            
    @QtCore.pyqtProperty(float, fset=setMasterVolume, notify=masterVolumeChanged)
    def masterVolume(self):
        return self._masterVolume

    # actualVolume
    def setActualVolume(self, actualVolume):
        if self._actualVolume != actualVolume:
            self._actualVolume = actualVolume
            self.actualVolumeChanged.emit(self._actualVolume)
            
    @QtCore.pyqtProperty(float, fset=setActualVolume, notify=actualVolumeChanged)
    def actualVolume(self):
        return self._actualVolume

    # waveformFile
    def setWaveformFile(self, waveformFile):
        if self._waveformFile != waveformFile:
            self._waveformFile = waveformFile
            self.waveformFileChanged.emit(self._waveformFile)
            
    @QtCore.pyqtProperty(str, fset=setWaveformFile, notify=waveformFileChanged)
    def waveformFile(self):
        return self._waveformFile





    def __repr__(self):
            return repr((self._name, self._index))
            
'''

Note if you get errors like: 
TypeError: unable to convert a Python 'int' object to a C++ 'QString' instance

Then @QtCore.pyqtProperty(str, notify=nameChanged)
                          ^^^
Is probably incorrect.

Attributes:
  name str
  filePath str
  index int
  length int

  start int
  stop int

  fadingIn int
  fadingOut int
  fadingStop int
  fadeIn int
  fadeOut int
  fadeStop int

  position int
  percent float

  playing int
  paused int
  loading int

  masterVolume float
  actualVolume float

  waveformFile str
'''