import QtQuick 1.1

Item {
  id: main
  
  property variant model:[];
  property int focusIndex: 0;
  property alias tabHeight: row.height;
  property string tabColor: "gray";
  property string activeTabColor: "orange";
  property string tabBackground: "darkgray";
  
  Rectangle {
    id: row;
    color: tabBackground;
    height: 18;
    width: parent.width;
    Row {
      x:-1 // to compensate for delegate's border.width
      height: parent.height;
      width: parent.width+1;
      Repeater {
        id: tab_repeater
        model: main.model.length
        delegate: Rectangle {
          id: this_delegate
          color: (focusIndex == index ? main.activeTabColor : main.tabColor)
          border.width: 1
          border.color: "black"
          height: parent.height - border.width;
          width: text.paintedWidth+8;
          property alias text: text.text
          Text {
            id: text
            x: 4
            text: ""
          }
          MouseArea {
            anchors.fill: parent;
            onClicked: {
              main.focusIndex = index;
            }
          }
          Component.onCompleted: {
            repeater.itemAt(index).item.tab = this_delegate
          }
        }
      }
    }  
  }
  
  Item {
    id: container
    anchors.top: row.bottom;
    height: parent.height-row.height;
    width: parent.width;
    Repeater {
      id: repeater
      model: main.model
      delegate: Item {
        property alias item: loader.item;
        height: parent.height;
        width: parent.width;
        visible: (main.focusIndex == index)
        Loader {
          id: loader
          height: parent.height;
          width: parent.width;
          sourceComponent: main.model[index];
        }
      }
    }
 }
}