import QtQuick 1.1

Item {
  id: checkbox
  
  property bool checked: false;
  property alias text: label.text;
  width: label.paintedWidth + box.width + label.anchors.leftMargin*2;
  height: label.paintedHeight;
  
  Rectangle {
    id: box
    height: label.paintedHeight-6
    width: label.paintedHeight-6
    anchors.bottom: label.baseline;
    border.width: 1
    border.color: "black"
    color: "white"
    Text {
      y:-2
      text: (checked ? "\u2713" : "")
    }
  }
  Text {
    id: label;
    anchors.left: box.right
    anchors.leftMargin: 3
  }
  MouseArea {
    anchors.fill: parent;
    onClicked: {
      checked = !checked;
    }
  }
}