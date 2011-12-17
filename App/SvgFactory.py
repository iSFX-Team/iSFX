from PyQt4 import QtCore, QtGui, QtDeclarative
from PyQt4.QtCore import QSize, QByteArray
from PyQt4.QtGui import QPixmap, QPainter
from PyQt4.QtSvg import QSvgRenderer
from PyQt4.QtDeclarative import QDeclarativeImageProvider
import sys


class SvgFactory(QDeclarativeImageProvider):
  def __init__(self):
    super(SvgFactory, self).__init__(QDeclarativeImageProvider.Pixmap)
    
  def requestPixmap(self, id, size, requestedSize):
    bytes = QByteArray(id)
    renderer = QSvgRenderer()
    renderer.load(bytes)
    size = renderer.defaultSize()
    image = QPixmap(requestedSize)
    painter = QPainter(image)
    renderer.render(painter)
    return image