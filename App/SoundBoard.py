import sys
import os
import random
import string
from PyQt4 import QtCore, QtGui, QtDeclarative
from PyQt4.QtDeclarative import QDeclarativeView
from xml.etree.ElementTree import * # todo: wtf ryan

from SoundData import SoundData
import iSFX


class SoundBoard(QtCore.QObject):
    
    soundsChanged = QtCore.pyqtSignal()
    sortedChanged = QtCore.pyqtSignal()
    
    supported_file_extensions = ['.AIFF', '.ASF', '.ASX', '.DLS', '.FLAC', '.FSB', '.IT', 
                                 '.M3U', '.MID', '.MOD', '.MP2', '.MP3', '.OGG', '.PLS', 
                                 '.RAW', '.S3M', '.VAG', '.WAV', '.WAX', '.WMA', '.XM']
   
    def load(self, filename):
      soundlist = []
      tree = parse(filename)
      root = tree.getroot()
      for node in root:
        s = SoundData()
        s.name = str(node.attrib.get("_name"))
        s.filePath = str(node.attrib.get("_filePath"))
        s.index = int(node.attrib.get("_index"))
        s.length = int(node.attrib.get("_length"))
        s.start = int(node.attrib.get("_start"))
        s.stop = int(node.attrib.get("_stop"))
        s.fadeIn = int(node.attrib.get("_fadeIn"))
        s.fadeOut = int(node.attrib.get("_fadeOut"))
        s.fadeStop = int(node.attrib.get("_fadeStop"))
        s.masterVolume = float(node.attrib.get("_masterVolume"))
        s.waveformFile = str(node.attrib.get("_waveformFile"))
        soundlist.append(s)
      return soundlist
    
    saveableAttributes = [ "_name",
                           "_filePath",
                           "_index",
                           "_length",
                           "_start",
                           "_stop",
                           "_fadeIn",
                           "_fadeOut",
                           "_fadeStop",
                           "_masterVolume",
                           "_waveformFile" ]
    
    def save(self, filename):
      root = Element("sounds")
      for sound in self._sorted:
        s = SubElement(root, "sound")
        for a in self.saveableAttributes:
          s.attrib[a]=str(getattr(sound, a))
      ElementTree(root).write(filename)
    
    def loadSoundsFromXML(self, filepath):
      sounds_from_xml = []

      try:
        sounds_from_xml = self.load("sounds.xml")
      except Exception as e:
        print("Exception while loading sounds.xml: ", e)
        if os.path.exists("sounds.xml"):
          i = 1
          new_file_name = "sounds.xml"
          while (os.path.exists(new_file_name)):
            new_file_name = "bad_sounds"+str(i)+".xml"
            i+=1
          print("Renaming bad 'sounds.xml' to '"+new_file_name+"'")
          sys.stdout.write('\a') # beep
          sys.stdout.flush()
          input("Press any Key to continue...")
          os.rename("sounds.xml", new_file_name)
        f = open('sounds.xml', 'w')
        f.write('<sounds></sounds>')
        f.close()
        try:
          sounds_from_xml = self.load("sounds.xml")
        except Exception as e:
          print("Exception while loading NEW sounds.xml...\nmaybe something else is wrong...\nException: ", e)
      return sounds_from_xml
    
    def loadSoundsFromFolder(self, path, sounds_from_xml):
      all_sounds = []
      skipping = 0
      if os.path.exists(path):
        files = os.listdir(path)
        print("Found", len(files), "files.")
        for f in files:
          (filename, extension) = os.path.splitext(f)
          if extension.upper() in self.supported_file_extensions:
            b = True
            for s in sounds_from_xml:
              #print (s.path, "vs.", os.path.join(path,f))
              if s.filePath == os.path.join(path,f):
                b = False
                skipping += 1
                #print("skipping", os.path.join(path,f), "... already in xml")
                break
            if (b):
              #print("Adding: "+os.path.join(path,f))
              s = SoundData()
              s.filePath = os.path.join(path,f)
              all_sounds.append(s)
      else:
        print("Warning ../../sounds not found.")
      print("skipping",skipping,"sounds because they were already loaded from xml")
      return all_sounds

    def __init__(self, system, folder, filename):
        super(SoundBoard, self).__init__()

        self.setObjectName('mainObject')
        random.seed()
        
        self._system = system
        #self.nullModel = SoundData("", -1); # not sure if this will be/is being used
        self._sounds = []
        self._sorted = []
        
        '''
        
        Load all of the SoundData from XML.
        Create SoundData entries for sound files in the sounds folder.
        Then create the iSFX::Sounds using system.
        Connect the signals between the two.
        
        '''
        
        loadedFromXML = self.loadSoundsFromXML(filename)
        loadedFromFolder = self.loadSoundsFromFolder(folder, loadedFromXML)
        loadedSounds = loadedFromXML+loadedFromFolder
        
        charBank = string.ascii_uppercase + string.ascii_lowercase + string.digits
          
        for entry in loadedSounds:
          #print("Loading: \'"+entry.filePath+"\' length: "+str(entry.length))
          random_waveform_path = "./waveforms"
          while os.path.exists(random_waveform_path):
            random_waveform_path = "./waveforms/"+''.join(random.choice(charBank) for x in range(20))+".svg"
            #print(random_waveform_path)
          entry.waveformPath = random_waveform_path;
          s = iSFX.Sound(system, entry.filePath, entry.length, entry.waveformPath)
          entry.setBackend(s)
          if (entry.length != 0):
            s.stream()
          else:
            s.load()
          self._sounds.append(entry)
        self.sort()
        
        # Code to create randomly sorted entries a-z
        # bank = []
        # for c in string.ascii_lowercase:
        #   bank.append(c)
        # names = []
        # for i in range(26):
        #   r = random.choice(bank)
        #   bank.remove(r)
        #   names.append(r)
        # for ii in range(len(names)):
        #   s = SoundData(names[ii], ii)
        #   #s.indexChanged.connect(self.sort)
        #   self._sounds.append(s)
        # self.sort()
    
    @QtCore.pyqtSlot(int, result=str)
    def getWaveData(self, channel):
      s = self._system.getWaveData(channel)
      return s

    @QtCore.pyqtSlot()
    def killAll(self):
      for s in self._sounds:
        s.doKill()
    
    @QtCore.pyqtSlot()
    def stopAll(self):
      for s in self._sounds:
        s.doStop()
    
    def sort(self):
      self._sorted = sorted(self._sounds, key=lambda s: s._index)
      for i in range(len(self._sorted)): # this is a quickfix. Problem was that all indexes were 0 initially
        self._sorted[i].index = i
      self.sortedChanged.emit()
      
    @QtCore.pyqtSlot(int)
    def promote(self, index):
      if index > 0:
        self._sorted[index].index -= 1
        self._sorted[index-1].index += 1
        self.sort()
      
    @QtCore.pyqtSlot(int)
    def demote(self, index):
      if index < len(self._sorted)-1:
        self._sorted[index].index += 1
        self._sorted[index+1].index -= 1
        self.sort()
     
    @QtCore.pyqtSlot(int, int)
    def move(self, old, new):
      if (old != new and new < len(self._sorted) and new >= 0 ):
        step = (-1,1)[old < new]
        index = self._sorted[old].index
        for i in range(old, new, step):
          self._sorted[i].index = self._sorted[i+step].index
        self._sorted[new].index = index
        self.sort()
    
    @QtCore.pyqtProperty(QtDeclarative.QPyDeclarativeListProperty, notify=sortedChanged)
    def sorted(self):
        return QtDeclarative.QPyDeclarativeListProperty(self, self._sorted)     
    
    def _sorted(self, row):
        return self._sorted[row]
    
    @QtCore.pyqtProperty(QtDeclarative.QPyDeclarativeListProperty, notify=soundsChanged)
    def sounds(self):
        return QtDeclarative.QPyDeclarativeListProperty(self, self._sounds)

    def _sound(self, row):
        return self._sounds[row]