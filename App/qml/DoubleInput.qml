import QtQuick 1.1
import "Root.js" as Root

Rectangle {
  id: container
  height: text_input.font.pixelSize+3;
  width: 60;
  
  color: "white";
  border.width: 1;
  border.color: "darkgray";
  
  property double value:0.0;
  property alias min: validator.bottom;
  property alias max: validator.top;
  
  // this is a quick-fix for a bug
  property int clicks: 0;
  property alias text_input: text_input;
  property alias text: text_input.text;
  
  signal accepted (double value);
  
  MouseArea {
    id:textinput_deselection
    enabled: false;
    parent: (enabled ? Root.get_root(container) : container)
    anchors.fill: parent;
    onPressed: {
      //console.log("deselect")
      if (clicks > 1) {
        mouse.accepted = false;
        text_input.focus = false;
        textinput_selection.enabled = true;
        enabled = false;
        if (text_input.acceptableInput) {
          container.accepted(parseFloat(text_input.text))
        }
      } else {
        clicks+=1;
      }
    }
  }
  TextInput{
    id: text_input
    selectByMouse: true;
    text: "0.00";//slider.value.toFixed(2);
    onAccepted: {
      container.accepted(parseFloat(text))
      text_input.focus = false;
      Root.get_root(text_input).forceActiveFocus();
    }
    onFocusChanged: {
      if (focus == true) {
        text_input.selectAll()
      }
    }
    validator: DoubleValidator{
      id: validator;
    }
  }
  MouseArea {
    id:textinput_selection
    anchors.fill: parent;
    onPressed: {
      if (clicks < 1) {
        clicks+=1;
      }
      //console.log("select")
      mouse.accepted = false;
      text_input.focus = true;
      textinput_deselection.enabled = true;
      enabled = false;
    }
  }
}