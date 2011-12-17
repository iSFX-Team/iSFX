import os
import sys

from PyQt4 import QtCore, QtGui, QtDeclarative
from PyQt4.QtGui import QApplication, QCursor
from PyQt4.QtDeclarative import QDeclarativeView
from xml.etree.ElementTree import *

from SoundData import SoundData
from SoundBoard import SoundBoard
from SvgFactory import SvgFactory
from WaveFactory import WaveFactory

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
  import iSFX64 as iSFX # i don't think this ever exists
else:
  import iSFX
    
def setCursor(e, s):
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

if __name__ == '__main__':
    
    app = QtGui.QApplication(sys.argv)
    canvas = QtDeclarative.QDeclarativeView()
    engine = canvas.engine()
    
    factory = SvgFactory()
    engine.addImageProvider("svg_from_string", factory);
    
    system = iSFX.System()
    soundboard = SoundBoard(system, "../../sounds", "sounds.xml")
    
    
    waveFactory = WaveFactory(soundboard._system.getWaveData)
    engine.addImageProvider("waveFactory", waveFactory);

    engine.rootContext().setContextObject(soundboard)
    canvas.setSource(QtCore.QUrl.fromLocalFile('qml/main.qml'))
    canvas.setResizeMode(QDeclarativeView.SizeRootObjectToView)
    engine.quit.connect(app.quit)
    
    rootObject = canvas.rootObject()
    rootObject.updateSignal.connect(system.Update)
    rootObject.setCursorSignal.connect(setCursor)
    
    canvas.setGeometry(QtCore.QRect(0, 0, rootObject.property("width"), rootObject.property("height")))
    canvas.setWindowTitle("iSFX")
    canvas.show()
    canvas.raise_()

    app.exec_()
    soundboard.save("sounds.xml")
