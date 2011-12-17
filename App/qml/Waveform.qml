import QtQuick 1.1

Item {
  id: container
  width: parent.width;
  height: parent.height;
  property alias waveformFilePath: waveform.source;
  property bool slidersVisible: true;
  //property alias cache: waveform.cache;
  
  property double start;
  property double stop;
  property double fadeIn;
  property double fadeOut;
  
  property double percent: 0;
  
  function bound(x, min, max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
  }
  
  function set_waveform(_x1, _x2, _len1, _len2) {
    //
    //  This if statement is to stop a rounding (or value) issue.
    //  When the sounds start, stop, fadeIn, or fadeOut change,
    //  the waveform is update to display the new value. But
    //  when the waveform is being dragged, it causes issues with
    //  setting the value you are editing twice.
    //
    //  To experience the issue, remove this if statement, click to edit
    //  a sound, then drag stop all the way to the right, resize the fadeOut
    //  to zero, then resize fadeIn to non-zero, finally move the start
    //  position towards the end of the sound. When the region starts hitting
    //  the stop point, it will move slower and start breaking the bound()
    //  due to error introduced by this function. Most likely originating in
    //  the update_waveform() function in EditSound.qml
    //
    if (!start_drag.dragging && !stop_drag.dragging && !stop_boundry_drag.dragging && !start_boundry_drag.dragging) {
      len1 = 1.0/container.width
      len2 = 1.0/container.width
      x1 = 0
      x2 = 1
      
      x1 = bound(_x1, 0, x2-len2-len1);
      x2 = bound(_x2, x1+len1+len2, 1);
      len1 = bound(_len1, 0, x2-len2-x1);
      len2 = bound(_len2, 0, x2-(x1+len1));
    }
  }
  
  function set_start(x) {
    x1 = bound(x, 0, x2-len2-len1);
    start = x1;
  }
  function set_stop(x) {
    x2 = bound(x, x1+len1+len2, 1);
    stop = x2;
  }
  function set_fadein(x) {
    len1 = bound(x, 0, x2-len2-x1);
    fadeIn = len1;
  }
  function set_fadeout(x) {
    len2 = bound(x, 0, x2-(x1+len1));
    fadeOut = len2;
  }
  
  property double x1:0;
  property double x2:1;
  property double len1:0.25;
  property double len2:0.25;

  Rectangle {
    id: background
    color: "lightgray"
    width: parent.width;
    height: parent.height;
    Image {
      id: waveform
      cache: false;
      asynchronous: true;
      source: "../blank.svg";
      width: parent.width;
      height: parent.height;
      sourceSize.width: width;
      sourceSize.height: height;
      
      //property string filepath: (waveformFilePath !== "") ? "../"+waveformFilePath : "";
      //onFilepathChanged: {
      //  source = 
      //}
      //onStatusChanged: {
      //  if (status == Image.Null || (status == Image.Ready && source !== filepath)) {
      //    source = filepath; 
      //  }
      //}
    }
  }
  
  function raise_z(a,b) {
    if (a.z < b.z) {
      var t = a.z;
      a.z = b.z;
      b.z = t;
    }
  }
  
  Item {
    id: slider_container;
    anchors.fill: parent;
    visible: slidersVisible
    
    
    Rectangle {
      x: x1*container.width;
      width: len1*container.width;
      height: parent.height;
      color: "blue"
      opacity: 0.4+(z-1)*0.1
      Draggable {
        id: start_drag
        anchors.fill: parent;
        onDragChanged: {
          set_start(drag.x/container.width);
        }
        onClicked: { raise_z(start_boundry, stop_boundry); }
      }
      z: start_boundry.z
    }
    Rectangle {
      id: start_boundry
      x: (x1+len1)*container.width;
      width: 1
      height: parent.height;
      color: "blue"
      z:1
      Rectangle {
        color: (start_boundry_drag.containsMouse && !start_boundry_drag.dragging  ? "blue" : "transparent")
        width: 5
        height: parent.height;
        x: -2
        Item {
          height: parent.height;
          width: parent.width+2;
          x:-1
          Draggable {
            anchors.fill: parent;
            id: start_boundry_drag
            target: parent.parent.parent;
            onDragChanged: {
              set_fadein((drag.x-x1*container.width)/container.width);
            }
            onClicked: { raise_z(start_boundry, stop_boundry); }
          }
        }
      }
    }
    Rectangle {
      x: (x2-len2)*container.width;
      width: len2*container.width;
      height: parent.height;
      color: "red"
      opacity: 0.4+(z-1)*0.1
      Draggable {
        id: stop_drag
        anchors.fill: parent;
        onDragChanged: {
          set_stop((drag.x+len2*container.width)/container.width);
        }
        onPressed: { raise_z(stop_boundry, start_boundry); }
      }
      z: stop_boundry.z
    }
    Rectangle {
      id: stop_boundry
      x: (x2-len2)*container.width;
      width: 1
      height: parent.height;
      color: "red"
      z:2
      Rectangle {
        color: (stop_boundry_drag.containsMouse && !stop_boundry_drag.dragging  ? "red" : "transparent")
        width: 5
        height: parent.height;
        x: -2
        Item {
          height: parent.height;
          width: parent.width+2;
          x:-1
          Draggable {
            id: stop_boundry_drag
            anchors.fill: parent;
            target: parent.parent.parent;
            onDragChanged: {
              set_fadeout((x2*container.width-drag.x)/container.width);
            }
            onPressed: { raise_z(stop_boundry, start_boundry); }
          }
        }
      }
    }
    Rectangle {
      x: percent/100.0*container.width;
      width: 1;
      height: parent.height;
      color: "black"
      z:3
    }
  }
}