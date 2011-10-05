import sys
import os
import string

if sys.platform == 'darwin':
  os.system("rm -f iSFX.so")
  os.system("rm -f libfmodex.dylib")
  os.system("ln -s ../Lib/Python/build/iSFX.so . &> /dev/null")
  os.system("ln -s ../FMOD/lib/libfmodex.dylib . &> /dev/null")
elif sys.platform == 'win64':
  os.system("")
import iSFX

from PyQt4.QtCore import QDateTime, QObject, QUrl, pyqtSignal
from PyQt4.QtGui import QApplication
from PyQt4.QtDeclarative import QDeclarativeView

system = iSFX.System();

songs = []
def StopAll():
  for s in songs:
    s.stop();

app = QApplication(sys.argv)

# Create the QML user interface.
view = QDeclarativeView()
view.setSource(QUrl('main.qml'))
view.setResizeMode(QDeclarativeView.SizeRootObjectToView)

rootObject = view.rootObject()
rootObject.updateSignal.connect(system.Update)
rootObject.escapeSignal.connect(StopAll)

view.setGeometry(20, 20, rootObject.property("width"), rootObject.property("height"))
view.show()
for i in range(36):
  rootObject.addElements()

def Print(index, mouse):
  print("Clicked: " + str(index) + " " + str(mouse.property("x")) + " " + str(mouse.property("y")))

  
i = 0
files = []
path = ""
if os.path.exists("sounds"):
  files = os.listdir("sounds")
  path = "sounds"
elif os.path.exists(os.path.join(os.pardir, "sounds")):
  files = os.listdir(os.path.join(os.pardir, "sounds"))
  path = os.path.join(os.pardir, "sounds")
elif os.path.exists(os.path.join(os.pardir, os.pardir, "sounds")):
  files = os.listdir(os.path.join(os.pardir, os.pardir, "sounds"))
  path = os.path.join(os.pardir, os.pardir, "sounds")
for f in files:
  if f == ".DS_Store": continue
  if f == "README": continue
  (c, r) = divmod(i,36)
  l = rootObject.getListContents(c)
  e = l[r]
  s = iSFX.Sound(system, os.path.join(path, f))
  print(os.path.join(path, f))
  s.fadeEntering(10000)
  s.setPercentCallback(e.setProgress)
  s.setNewNameCallback(e.setName)
  s.setInStateCallback(e.setState)
  s.setPlayingCallback(e.nowPlaying)
  e.buttonSignal.connect(s.playStop)
  e.setText(s.getName())
  songs.append(s)
  i+=1

app.exec_()
