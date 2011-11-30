import QtQuick 1.1

MouseArea {
  id: ma
  property alias text: text.text;
  property string url;
  property string colorHover: "blue"
  property string colorNormal: "blue"
  Text { 
    id: text;
    color: (ma.containsMouse ? colorHover : colorNormal)
    font.underline: ma.containsMouse
  }
  width: text.paintedWidth;
  height: text.paintedHeight;
  hoverEnabled: true;
  onClicked: {
    if (url !== "") {
      Qt.openUrlExternally(url);
    }
  }
}