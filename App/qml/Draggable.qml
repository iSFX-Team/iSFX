import QtQuick 1.1

MouseArea {
  id:area;
  
  hoverEnabled: true
  acceptedButtons: Qt.LeftButton | Qt.RightButton
  
  property bool clickAfterDrag: true;
  property bool dragging: false;
  property variant target: area.parent;
  property variant grab: Qt.point(-1, -1);
  property variant drag: Qt.point(-1, -1);
  property bool requireHold: false;
  
  
  onPressed: {
    if (!requireHold || mouse.button == Qt.RightButton) {
      dragging = true;
      grab = mapToItem(area, mouse.x, mouse.y)
    }
  }
  onPressAndHold: {
    if (requireHold) {
      dragging = true;
      grab = mapToItem(area, mouse.x, mouse.y)
    }
  }
  onPositionChanged: {
    if (dragging) {
      if (!pressed) { 
        released(mouse);// force onReleased to fire. Workaround for QML bug
        return;
      }
      drag = mapToItem(target.parent, mouse.x-grab.x, mouse.y-grab.y);
    }
  }
  onReleased: {
    dragging = false;
  }
}