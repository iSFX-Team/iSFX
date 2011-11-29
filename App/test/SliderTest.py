import sys
import os
import string

from PyQt4.QtCore import Qt, QObject, QUrl, pyqtSignal
from PyQt4.QtGui import QApplication, QCursor
from PyQt4.QtDeclarative import QDeclarativeView
    
app = QApplication(sys.argv)

# Create the QML user interface.
view = QDeclarativeView()
  

view.setSource(QUrl('SliderTest.qml'))
view.setResizeMode(QDeclarativeView.SizeRootObjectToView)

rootObject = view.rootObject()

view.setGeometry(20, 20, rootObject.property("width"), rootObject.property("height"))
view.setWindowTitle("SliderTest")
view.showMaximized()
view.raise_()

app.exec_()
