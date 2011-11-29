import sys
import os
import string

if sys.platform == 'darwin':
  os.system("rm -f iSFX.so")
  os.system("rm -f libfmodex.dylib")
  os.system("ln -s ../Lib/Python/build/iSFX.so . &> /dev/null")
  os.system("ln -s ../FMOD/lib/libfmodex.dylib . &> /dev/null")
elif sys.platform[0:3] == 'win':
  os.system("copy /y ..\\FMOD\\lib\\fmodex64.dll .\\fmodex64.dll >nul 2>nul")
  #if os.path.exists(os.path.join(os.pardir, "Lib", "Python", "build", "iSFX.pyd")):
  os.system("copy /y  ..\\Lib\\Python\\build\\iSFX.pyd .\\iSFX.pyd >nul 2>nul")
else:
  print("Add support for your platform " + sys.platform + " in main.py.")

if sys.platform == 'win64':
  import iSFX64 as iSFX
else:
  import iSFX

from PyQt4.QtCore import Qt, QObject, QUrl, pyqtSignal
from PyQt4.QtGui import QApplication, QCursor
from PyQt4.QtDeclarative import QDeclarativeView
from xml.etree.ElementTree import *

class Sound:
    def __init__(self, display_name="", file_path="", length=33554432, start=0, stop=0, master_volume=1, fade_in=0, fade_stop=0, fade_out=0):
        self.display_name = display_name
        self.file_path = file_path
        self.length = length
        self.start = start
        if (stop != 0):
          self.stop = stop
        else:
          self.stop = self.length
        self.fade_in = fade_in
        self.master_volume = master_volume
        self.fade_stop = fade_stop
        self.fade_out = fade_out
        
    def setOption(self, p, v):
      setattr(self, p, v)



def StopAll():
  for s in sounds:
    s.performAction("kill");
    
def FadeAll():
  for s in sounds:
    s.performAction("stop");
    
app = QApplication(sys.argv)

# Create the QML user interface.
view = QDeclarativeView()

def raise_():
  view.raise_()

def quit():
  app.closeAllWindows()
  
def toggle_fullscreen():
  if not (view.windowState() & 0x00000004):
    view.showFullScreen()
  else:
    view.showNormal()
    
def set_cursor(e, s):
  if (s == "pointing hand"):
    e.setCursor(QCursor(Qt.PointingHandCursor))
  elif (s == "grabbing hand"):
    e.setCursor(QCursor(Qt.ClosedHandCursor))
  elif (s == "open hand"):
    e.setCursor(QCursor(Qt.OpenHandCursor))
  elif (s == "normal"):
    e.setCursor(QCursor(Qt.ArrowCursor))
  else:
    print(s)

'''
Execution Starts Here
'''
system = iSFX.System();

view.setSource(QUrl('qml/main.qml'))
view.setResizeMode(QDeclarativeView.SizeRootObjectToView)

rootObject = view.rootObject()
rootObject.raise_.connect(raise_)
rootObject.quit.connect(quit)
rootObject.toggle_fullscreen.connect(toggle_fullscreen)
rootObject.set_cursor.connect(set_cursor)
rootObject.updateSignal.connect(system.Update)
rootObject.escapeSignal.connect(StopAll)
rootObject.spaceSignal.connect(FadeAll)

view.setGeometry(20, 20, rootObject.property("width"), rootObject.property("height"))
view.setWindowTitle("iSFX")
view.showMaximized()
view.raise_()
for i in range(26):
  rootObject.add_elements()
  
supported_file_extensions = ['.AIFF', '.ASF', '.ASX', '.DLS', '.FLAC', '.FSB', '.IT', 
                             '.M3U', '.MID', '.MOD', '.MP2', '.MP3', '.OGG', '.PLS', 
                             '.RAW', '.S3M', '.VAG', '.WAV', '.WAX', '.WMA', '.XM']


xm = SoundsXML()
#xm.saveXML(sounds, "test.xml")
sounds_from_xml = []

try:
  sounds_from_xml = xm.loadXML("sounds.xml")
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
    sounds_from_xml = xm.loadXML("sounds.xml")
  except Exception as e:
    print("Exception while loading NEW sounds.xml...\nmaybe something else is wrong...\nException: ", e)
    
  
all_sounds = []

for s in sounds_from_xml:
  all_sounds.append(s)

sounds = []
files = []

path = os.path.join(os.pardir, os.pardir, "sounds")

if os.path.exists(path):
  files = os.listdir(path)
  print("Found", len(files), "files.")
  for f in files:
    (filename, extension) = os.path.splitext(f)
    if extension.upper() in supported_file_extensions:
      b = True
      for s in sounds_from_xml:
        #print (s.path, "vs.", os.path.join(path,f))
        if s.file_path == os.path.join(path,f):
          b = False
          print("skipping", os.path.join(path,f), "... already in xml")
          break
      if (b):
        print("Adding: "+os.path.join(path,f))
        all_sounds.append(Sound(file_path=os.path.join(path,f)))
        print("checking: "+all_sounds[len(all_sounds)-1].file_path)
else:
  print("Warning ../../sounds not found.")

xm.saveXML(all_sounds, "sounds.xml")

i = 0
for entry in all_sounds:
  try:
    print("Loading: \'"+entry.file_path+"\'")
    s = iSFX.Sound(system, entry.file_path)
  except Exception as e: # this doesn't catch the exceptions inside iSFX work
    print("exception:", e)
    break;
  
  (c, r) = divmod(i,26)
  
  if rootObject == None: print("QML parsing failed due to errors.")
  column = rootObject.get_contents(c)
  if column == None:
    print("Column", c, "does not exist.")
    break;
  row = column[r]
  if row == None:
    print("Row", r, "does not exist.")
    break;
  draggable = row.get_content()
  if row == None:
    print("Draggable object at (",c,r,") does not exist.")
    break;
  elem = draggable.get_object()
  if row == None:
    print("Sound object at (",c,r,") does not exist.")
    break;
  
  
  s.setStateChangeCallback(elem.updateState)
  #s.setStateChange_uint32_t_Callback(elem.updateState)
  #s.setStateChange_double_Callback(elem.updateState)
  elem.optionChanged.connect(s.setOption)
  elem.optionChanged.connect(entry.setOption)
  elem.actionSignal.connect(s.performAction)
  
  
  for attr in entry.__dict__:
    elem.changeOption(attr, getattr(entry, attr))
  
  sounds.append(s)
  i+=1

app.exec_()

xm.saveXML(all_sounds, "sounds.xml")