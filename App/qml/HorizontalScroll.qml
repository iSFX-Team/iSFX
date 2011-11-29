import QtQuick 1.0

Flickable {
  id: root
  property alias speed: root.maximumFlickVelocity;
  property double value: contentX/root.width;
  boundsBehavior: Flickable.StopAtBounds;
  
  width: parent.width;
  anchors.fill: parent;
  flickableDirection: Flickable.HorizontalFlick;
  contentWidth: parent.width*2
  
  function set_position(p) {
    //console.log("set_position")
    contentX = p;
  }
}