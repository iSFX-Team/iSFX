import QtQuick 1.1
import "Hotkey.js" as Hotkey
import "Theme.js" as Theme

Rectangle {
  id: sound
  
  signal editClicked ();
  property bool beingEdited: false;
  
  property variant viewData;
  property variant placeholder: viewData.placeholders[modelData.index];
  
  // note if sound height changes, this needs to change. 
  //  e.g. if Categories are added.
  property int row: y/(sound.height+viewData.rowPadding);
  property int column: x/(sound.width+viewData.columnPadding);
  
  property int index: -1; // this index is only used to index into model \/
  property variant model: undefined;
  property variant modelData: (index !== -1 ? model[index] : nullModel);
  
  property bool shouldBind: placeholder !== undefined && !dragArea.dragging;
  Binding { when: shouldBind; target: sound; property: "x"; value: placeholder.x+viewData.columnPadding; }
  Binding { when: shouldBind; target: sound; property: "y"; value: placeholder.y+viewData.rowPadding; }
  Binding { when: shouldBind; target: placeholder; property: "width"; value: width+viewData.columnPadding; }
  Binding { when: shouldBind; target: placeholder; property: "height"; value: height+viewData.rowPadding; }
  Binding { when: shouldBind; target: placeholder; property: "target"; value: index; }
  
  z: (dragArea.dragging ? 1 : 0)
  visible: x+width-1 < parent.width
  
  height: 20
	width: (parent.width-viewData.columnPadding-1)/5-viewData.columnPadding;
  color: "transparent"
  border.width: 1;
  border.color: "black"
  
  Rectangle {
    id:container
    color:  (dragArea.dragging ? Theme.snd_bkg_drag : Theme.snd_bkg)
    height: parent.height
    width: parent.width
    
    TextBox {
      id: hotkey_button
      width: parent.height
      height: parent.height
      
      text.color: "black";
      
      text.text: Hotkey.fromIndex(row);
      text.opacity: (viewData.focusedColumn == sound.column)
      
      background.color: "white";
      background.opacity: 0.7;
    }
    AnimatedGradient{
      anchors.left: hotkey_button.right;
      height: 80;
      width: parent.height;
      transform: [ Rotation { angle: 270 }, Translate { y: height } ]
      colors: [ "#FF405040", "#00405040",
                "#FF404040", "#00404040",
                "#FF666666", "#00666666" ]
      index: 4;//(modelData.name === "" && modelData.filePath !== "" )
               //? 0 : (sound.column_focused || sound.mouse_focused ? 2 : 4)
      startPercentage: 0.40;
      pauseDuration: 200;
      animationLength: 300;
    }
    Text {
      id: label
      height: parent.height
      anchors.left: hotkey_button.right;
      anchors.right: buttons.right;
      anchors.leftMargin: 5;
      font.pixelSize: 11;
      verticalAlignment: Text.AlignVCenter;
      text: modelData.name
      color: Theme.snd_label;
      clip: true;
    }
    AnimatedGradient{
      anchors.right: buttons.right;
      height: 50;
      width: parent.height;
      transform: [
        Rotation { angle: 90 },
        Translate { x: height }
      ]
      colors: [ "#FF405040", "#00405040",
                "#FF404040", "#00404040",
                "#FF666666", "#00666666" ]
      index: 4;//(modelData.name === "" && modelData.filePath !== "" )
               //? 0 : (sound.column_focused || sound.mouse_focused ? 2 : 4)
      startPercentage: 0.40;
      pauseDuration: 200;
      animationLength: 300;
    }
    ProgressBar {
      id:progress
      percent: modelData.percent;
      anchors.left: container.left;
      height: sound.height;
      min_width: hotkey_button.width;
      max_width: sound.width;
      zero_width: ( beingEdited ? min_width : 0);
      opacity: (beingEdited ? 0.5 : 0.200);
      color: (beingEdited ? "orange" : "#00ff00" )
    }
    
    Item {
      id:buttons;
      width: 5+4+edit_rect.width/*+5+volume.width*/;
      anchors.right: container.right;
      height: parent.height;
      
      opacity: beingEdited || dragArea.containsMouse || edit_button.containsMouse
      Behavior on opacity {
        SequentialAnimation {
          PauseAnimation { duration: 200 }
          NumberAnimation { duration: 300 }
        }
      }
      Rectangle {
        id: edit_rect
        color: Theme.snd_edit_bkg;
        width: parent.height;
        height: parent.height;
        x: parent.width-width;
        
        Image{
	        height: parent.height;
	        width: parent.width;
	        source:"editIcon.png";
	        sourceSize.width:  parent.width;
	        sourceSize.height: parent.height;
        }
      }
    }
    MouseArea {
      id: edit_button
      hoverEnabled: true
      anchors.fill: parent;
      onClicked: {
        console.log("clicked")
        if (edit_button.containsMouse) {
          sound.editClicked()
        }
      }
    }
    
    states: [
        State {
            when: beingEdited
            name: "active"
            PropertyChanges { target: label; text: " * " + modelData.name}
            PropertyChanges { target: edit_rect; color: Theme.snd_edit_bkg_editing}
        }
    ]
    Draggable {
      id: dragArea
      height: parent.height;
      width: parent.width-edit_rect.width;
      target: sound; // should be the outermost component
      requireHold: true
      
      function bound(x, min, max) {
        if (x < min) return min;
        if (x > max) return max;
        return x;
      }
      onClicked: {
        if (mouse.button != Qt.RightButton) {
          modelData.doPlayStop();
          sound.editClicked();
        }
      }
      
      onDragChanged: {
        if (dragging) {
          sound.x = bound(drag.x, 0, sound.parent.width-sound.width)
          sound.y = bound(drag.y+grab.y-sound.height/2, 0, sound.parent.height-sound.height)
          var o = placeholder.parent.childAt(drag.x+grab.x, drag.y+grab.y)
          //if (o != null) {
          //  viewData.insertAbove = o.index
          //} else {
          //  viewData.insertAbove = -1
          //}
        
          var o = placeholder.parent.childAt(drag.x+grab.x, drag.y+grab.y)
          if (o !== null) {
            move(o.index, modelData.index);
          }  
        }
      }
      onReleased: {
        drag = Qt.point(-1,-1)
        grab = Qt.point(-1,-1)
        var o = placeholder.parent.childAt(drag.x+grab.x, drag.y+grab.y)
        if (o !== null) {
          move(o.index, modelData.index);
        }
        //viewData.insertAbove = -1
      }
    }
  }
}
