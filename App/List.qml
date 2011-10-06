import QtQuick 1.0
import "List.js" as List

Rectangle {
  id: container;
  property string uid;
  property bool focused: false;
  
  width: 145;
  height: 36*17;
  color: "#606060";
  
  Behavior on opacity { 
    PropertyAnimation {easing.type: Easing.InOutQuad; duration:500 }
  }
  
  Component {
    id:draggable_delegate
    
    ////////////////////////////////////////////////////////////////////////////
    ///
    ///   Sound Entry Delegate
    ///
    Rectangle {
      id:rect;
      
      width:parent.width-1;
      height:17;
      x:1
      y:index*height;
      color: "#404040";
      border.width:1;
      border.color:"#202020";
      
      property int sound_state;
      property int index;
      property alias text: label.text;
      
      signal buttonSignal(string mod);
      //onActivated: {
      //  console.log(hotkey.text + ": " + mod)
      //}
      
      Rectangle {
        y:1
        id: hotkey_background;
        color: "white";
        opacity: 0.7;
        width: parent.height-1;
        height: parent.height-1;
        MouseArea {
          id:hotkey_button
          anchors.fill: parent;
          onClicked: {
            rect.buttonSignal("click " + mouse.x + "," + mouse.y)
          }
        }
      } // Rectangle
      Text {
        y:1
        id: hotkey;
        width: parent.height-1;
        anchors.left: hotkey_background.left;
        text: indexToAscii(index);
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        color: "black";
        opacity: !!container.focused
      } // Text
      Rectangle {
          id: progress
          anchors.left: hotkey_background.right;
          color: "#00ff00"
          property double percent: 0;
          width: (parent.width-hotkey_background.width)*Math.min(1.0, Math.abs(percent/100));
          y:1
          height: parent.height-1;
          opacity: 0.300;
          
          states: [
            State {
              name: "loading"; when: sound_state == 1
              PropertyChanges { target: progress; 
                color: "orange";
                percent: 100;
              }
              PropertyChanges { target: load_animation; running: true; }
            },
            State {
              name: "stopped";
              PropertyChanges { target: progress; 
                color: "red";
                percent: 100;
              }
              PropertyChanges { target: progress; state: true; }
            }
          ]
          
          SequentialAnimation {
            id:load_animation
            PropertyAnimation { target: progress; property: "opacity"; to: 0.2; easing.type: Easing.Linear; duration: 500 }
            //SequentialAnimation {
            //  id:inner_loop
            //  loops: Animation.Infinite
            //  PropertyAnimation{ target: progress; property: "opacity";  from: 0.2; to: 0.3; easing.type: Easing.Linear; duration: 500}
            //  PropertyAnimation{ target: progress; property: "opacity";  from: 0.3; to: 0.2; easing.type: Easing.Linear; duration: 500}
            //}
          }
         
          transitions: [
            Transition {
              from: "loading";
              SequentialAnimation {
                ParallelAnimation {
                  PropertyAnimation { target: progress; property: "opacity"; to: 0.8; easing.type: Easing.Linear; duration: 200 }
                  PropertyAnimation { target: progress; property: "color"; from: "orange"; to: "green"; easing.type: Easing.Linear; duration: 200 }
                }
                ParallelAnimation {
                  PropertyAnimation { target: progress; property: "opacity"; to: 0.0; easing.type: Easing.Linear; duration: 750 }
                  PropertyAnimation { target: progress; property: "percent"; from: 100; to: 0; easing.type: Easing.Linear; duration: 750 }
                }
                PropertyAction { target: progress; property:"opacity"; value: 0.30; }
              }
            },
            Transition {
              from: "stopped";
              SequentialAnimation {
                ParallelAnimation {
                  PropertyAnimation { target: progress; property: "opacity"; to: 0.8; easing.type: Easing.Linear; duration: 100 }
                  PropertyAnimation { target: progress; property: "color"; from: "green"; to: "red"; easing.type: Easing.Linear; duration: 100 }
                }
                ParallelAnimation {
                  PropertyAnimation { target: progress; property: "opacity"; to: 0.0; easing.type: Easing.Linear; duration: 750 }
                  PropertyAnimation { target: progress; property: "percent"; from: 100; to: 0; easing.type: Easing.Linear; duration: 750 }
                }
                PropertyAction { target: progress; property:"opacity"; value: 0.30; }
                PropertyAction { target: progress; property:"color"; value: "green"; }
              }
            }
          ]
      }
      Text {
        y:1
        id:label;
        width:parent.width-hotkey_background.width;
        height:parent.height-1;
        anchors.left: hotkey_background.right;
        anchors.leftMargin: 5;
        font.pixelSize: 11
        verticalAlignment: Text.AlignVCenter;
        text: "";
        color:"white";
      }
      ///
      ////////////////////////////////////////////////////////////////////////////
      
      signal clicked (int index, variant mouse);
      
      MouseArea {
        id:mouseArea;
        anchors.fill: parent;
        //preventStealing: true;
        
        property bool dragging: false;
        property variant dragpos;
        
        //onClicked: { rect.clicked(index, mouse); }
        onClicked: {
          var p = mapToItem(hotkey_button, mouse.x, mouse.y);
          if (p.x > 0 && p.y > 0 && p.x < rect.width && p.y < hotkey_button.height) {
            hotkey_button.clicked(mouse);
          } else {
            rect.clicked(index, mouse);
          }
        }
        onPressAndHold: { 
          dragging = true;
          dragpos = {x:mouse.x, y:mouse.y};
          rect.x = container.x + 17;
        }
        onReleased: { dragging = false;}
        onPositionChanged: {
          if (dragging) {
            List.checked_move(index, ((rect.y+mouse.y)/rect.height << 0));
            rect.y = rect.y+mouse.y-dragpos.y;//-rect.height/2;
            rect.x = rect.x+mouse.x-dragpos.x+17;
          }
        }
      }
      
      states: [
        State {
          name: "" // base state
          PropertyChanges { target: rect; parent: container }
          PropertyChanges { target: rect; y: rect.index*rect.height } // very important
          PropertyChanges { target: rect; x: 1 }
        },
        State { // TODO make container.parent something controllable
          name: "dragging"
          when: mouseArea.dragging
          PropertyChanges { target: rect; opacity: 0.90 }
          PropertyChanges { target: rect; width: container.width }
          PropertyChanges { target: rect; parent: container.parent }
          PropertyChanges { target: rect; z: 3 }
          PropertyChanges { target: container; opacity: 0.75}
        }
      ]
      
      // Functions in the Delegate
      function setProgress(pct) { progress.percent = pct; }
      function setName(name) { text = name; }
      function getProgress() { return progress.percent; }
      function setText(t) { text = t; }
      function getText() { return text; }
      function nowPlaying(b) {
        if (b) progress.state = "";
        else progress.state = "stopped";
      }
      function setState(s) {
        //READY: 0
        //LOADING: 1
        //ERROR: 2
        //CONNECTING: 3
        //BUFFERING: 4
        //SEEKING: 5
        //PLAYING: 6
        //SETPOSITION: 7
        sound_state = s;
      }
      function getState() { return sound_state; }
    }
  }
  
  states: [
    State { // TODO make container.parent something controllable
      name: "unfocused"
      when: !container.focused
      PropertyChanges { target: container; opacity: 0.80 }
    }
  ]

  function indexToQwerty(i) {
    return "1234567890qwertyuiopasdfghjklzxcvbnm".charAt(i);
  }
  function qwertyToIndex(k) {
    return "1234567890qwertyuiopasdfghjklzxcvbnm".indexOf(k);
  }
  function indexToAscii(i) {
    return String.fromCharCode(i < 10 ? i+48 : i+87);
  }
  function asciiToIndex(k) {
    var i = k.charCodeAt(0);
    return (i < 58 ? i-48 : i-87);
  }
  
  function addElement(s) {
    if (s === undefined) s = {}
    List.append(draggable_delegate.createObject(container, s));
  }
  
  function getListContents() {
    return List.contents;
  }
}