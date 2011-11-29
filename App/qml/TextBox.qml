import QtQuick 1.0

Item {
  property alias background: background;
  property alias text: text;
  Rectangle {
    id: background
    anchors.fill: parent;
  }
  Text {
    id: text
    anchors.fill: parent;
    horizontalAlignment: Text.AlignHCenter;
    verticalAlignment: Text.AlignVCenter;
  }
}