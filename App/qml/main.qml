import QtQuick 1.1
import "Theme.js" as Theme
import "Hotkey.js" as Hotkey

Rectangle {
  id: main
  color: "grey"
  width: 1200; 
  height: 700;
  
  focus: true;
  Keys.onUpPressed: { edit_window.do_promote() }
  Keys.onDownPressed: { edit_window.do_demote() }
  Keys.onSpacePressed: { stopAll() }
  Keys.onEscapePressed: { killAll() }
  Keys.onPressed: {
    console.log(event.key)
    if (event.key >= 16777264 && event.key <= 16777271) {
      ol.viewData.focusedColumn = event.key-16777264;
      return;
    }
    if (event.key <= 255) {
      var c = String.fromCharCode(event.key).toLowerCase();
      if (Hotkey.toIndex(c) >= 0) {
        console.log(Hotkey.toIndex(c))
        if (Hotkey.toIndex(c) < ol.viewData.height/(20+ol.viewData.rowPadding))
        sorted[Hotkey.toIndex(c)+(ol.viewData.height/(20+ol.viewData.rowPadding)*ol.viewData.focusedColumn)].doPlayStop();
      }
    }
  }
  
  signal setCursorSignal (string cursor);
  signal updateSignal;
  Timer {
    id:updateTimer
    interval: 15
    repeat: true
    onTriggered: {
      updateSignal();
    }
  }
  Component.onCompleted: {
    updateTimer.start();
  }
  
  Column {
    anchors.fill: parent;
    
    Row {
      height: parent.height-edit_window.height;
      width: parent.width;
      
      Column {
        width: parent.width-sideButtons.width;
        height: parent.height;
        Row {
          id:topButtons
          width: parent.width;
          height: 20;
          Repeater {
            model: 5 // Makes 5 buttons
            delegate: Button {
            		buttonText: "F"+(index+1)
            		buttonColor: "#A0A0A0"
            		textColor: "#404040"
            		width: parent.width/5;
                height: parent.height;
                radius: 0
            		onButtonClicked: { ol.viewData.focusedColumn = index }
            }
          }
        }
        OrderedList {
          id: ol
          color: Theme.sl_bkg;
          height: parent.height-topButtons.height;
          width: parent.width;
          count: sounds.length;
          placeholder: Rectangle {
            property variant target: undefined;
            property variant viewData;
            property int index: -1;
            color: Theme.snd_plc_bkg;
          }
          delegate: Sound {
            id: sound
            beingEdited: edit_window.modelData == modelData
            model: sounds
            onEditClicked: {
              if (edit_window.modelData == modelData) {
                edit_window.modelData = undefined;
              } else {
                edit_window.modelData = model[index];
                edit_window.index = index;
              }
            }
          }
        }
      }
      Rectangle {
        id: sideButtons;
        color: "blue"
        width: 20;
        height: parent.height;
        Text { text: "sideButtons" }
      }
    }
    EditSound {
      id: edit_window
      width: parent.width;
      height: 180;
    }
  }
}