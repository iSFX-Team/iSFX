import QtQuick 1.0
import "Root.js" as Root

Item {
  id:container
  property double value: (slider.x/(container.width-slider.width));
  property bool dragging: false;
  property int grab_x;
  signal exited;
  
  function destroyThis() {
    container.destroy();
  }
  function set_value(x) {
    slider.set_position((container.width-slider.width)*(x));
  }
  
  onDraggingChanged: {
    if (dragging) {
      backdrop.on();
    } else {
      backdrop.fadeOut();
    }
  }
  
  Rectangle {
    height: parent.height-4;
    width: parent.width;
    color:"white";
    
    HorizontalScroll {
      id:scroll;
      anchors.fill: parent;
      speed: 250;
      
      onMovementEnded: {
        container.dragging = false;
      }
      onMovementStarted: {
        container.dragging = true;
      }
      onValueChanged: {
        slider.set_position((container.width-slider.width)*scroll.value)
      }
    }
    
    Rectangle {
      color: "black";
      height: parent.height;
      width: 2;
      x: (parent.width*0.25) >> 0
    }
    Rectangle {
      color: "black";
      height: parent.height;
      width: 2;
      x: (parent.width*0.5) >> 0
    }
    Rectangle {
      color: "black";
      height: parent.height;
      width: 2;
      x: (parent.width*0.75) >> 0
    }
    Rectangle {
      id:shaft;
      width: slider.x+slider.width/2;
      height: parent.height;
      y: 0;
      x: 0;//slider.x+slider.width/2
      opacity:.9;
      color: "orange"
    }
    Rectangle {
      id:slider;
      function set_position(x) {
        if (x < 0) {
          slider.x = 0;
        } else if (x+slider.width > container.width) {
          slider.x = container.width-slider.width;
        } else {
          slider.x = x;
        }
      }
      
      height: parent.height+4;
      width: 6;
      y: -2;
      x: 0;
      //radius: 5;
      color: container.dragging ? "red" : "black"
      Rectangle {
        id: backdrop;
        opacity: 0;
        function fadeOut() {
          animation.start();
        }
        function on() {
          animation.stop();
          backdrop.opacity = 0.8;
        }
        SequentialAnimation {
          id: animation;
          PauseAnimation { duration: 200 }
          PropertyAnimation { target: backdrop; property: "opacity"; to: 0; duration: 300 }
        }
    
        y: -label.paintedHeight-6;
        x: -label.paintedWidth/2+parent.width/2-2;
        color: "white"
        width: label.paintedWidth+4;
        height: label.paintedHeight+4;
        radius: 5
        Text {
          x:2
          y:2
          id: label;
          color: "black"
          text: value.toFixed(2);//.toString().substring(0,4);
        }
      }
    }
  }
  MouseArea {
    id:area;
    anchors.fill: parent;
    hoverEnabled: true;
    onPressed: {
      container.dragging = true;
      var p = mapToItem(slider, mouse.x, 0)
      if (p.x < 0 || p.x > slider.width) {
        if (mouse.x > 0 && mouse.x+slider.width < container.width) {
          slider.set_position(mouse.x-slider.width/2);
          scroll.set_position(mouse.x-slider.width/2);
          grab_x = slider.width/2;
        }
      } else {
        grab_x = p.x;
      }
    }
    onExited: { container.exited() }
    onPositionChanged: {
      if (container.dragging) {
        var p = mapToItem(container, mouse.x-grab_x, 0)
        slider.set_position(p.x);
        scroll.set_position(p.x);
      }
    }
    onReleased: {
      container.dragging = false;
    }
  }
}