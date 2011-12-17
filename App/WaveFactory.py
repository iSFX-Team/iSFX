from PyQt4 import QtCore, QtGui, QtDeclarative
from PyQt4.QtCore import QSize, QByteArray
from PyQt4.QtGui import QPixmap, QPainter, QColor
from PyQt4.QtSvg import QSvgRenderer
from PyQt4.QtDeclarative import QDeclarativeImageProvider
import sys


class WaveFactory(QDeclarativeImageProvider):
  def __init__(self, callback):
    self._callback = callback
    #self._iter = 0
    super(WaveFactory, self).__init__(QDeclarativeImageProvider.Pixmap)
    
  def requestPixmap(self, id, size, requestedSize):
    head = """<svg
         viewBox=\"0 -1 512 2\"
         xmlns=\"http://www.w3.org/2000/svg\"
         xmlns:xlink=\"http://www.w3.org/1999/xlink\"
         preserveAspectRatio=\"none\">\n"""
    
    s1 = self._callback(0)
    s2 = self._callback(1)
    beginpoly = "<polyline points=\""
    def endpoly(color):
      return "\" style=\"fill:none;stroke:"+color+";stroke-width:0.03\"></polyline>"
    bytes = QByteArray(head + beginpoly + s1 + endpoly("green") + beginpoly + s2 + endpoly("red") + "</svg>")
    #self._iter = (self._iter + 1) % 256
    #f = open('./waveforms/instantaneousWaveform%03d.svg' % self._iter, 'w')
    #f.write(head + beginpoly + s1 + endpoly("green") + beginpoly + s2 + endpoly("red") + "</svg>")
    #f.close()
    renderer = QSvgRenderer()
    worked = renderer.load(bytes)
    if not worked:
      print("SVG parse failed.")
    size = renderer.defaultSize()
    image = QPixmap(requestedSize)
    image.fill(QColor(0,0,0))
    painter = QPainter(image)
    renderer.render(painter)
    return image