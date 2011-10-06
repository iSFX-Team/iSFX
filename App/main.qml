import QtQuick 1.0
import "."

Rectangle {
    id: container;
    width: 1200;
    height: list1.height;
    color: "#C0C0C0";
    
    property int columnFocus: 0;
    
    signal updateSignal;
    signal escapeSignal;
    Timer {
      id:updateTimer
      interval: 12
      repeat: true
      onTriggered: {
        updateSignal();
      }
    }
    
    Component.onCompleted: {
      updateTimer.start();
    }
    
    Item {
      focus:true;
      Keys.enabled: true;
	  Keys.onEscapePressed: {
		escapeSignal();
	  }
      Keys.onPressed: {
        if (event.key >= 16777264 && event.key <= 16777271) {
          columnFocus = event.key-16777264;
          return;
        }
        if (event.key <= 255) {
          var c = String.fromCharCode(event.key).toLowerCase();
          if (qwertyToIndex(c) >= 0) {
            var o;
            if (columnFocus == 0) {
              o = list1.getListContents()[asciiToIndex(c)];
            } else if (columnFocus == 1) {
              o = list2.getListContents()[asciiToIndex(c)];
            } else if (columnFocus == 2) {
              o = list3.getListContents()[asciiToIndex(c)];
            } else if (columnFocus == 3) {
              o = list4.getListContents()[asciiToIndex(c)];
            }// else if (columnFocus == 4) {
            //  o = list5.getListContents()[asciiToIndex(c)];
            //} else if (columnFocus == 5) {
            //  o = list6.getListContents()[asciiToIndex(c)];
            //} else if (columnFocus == 6) {
            //  o = list7.getListContents()[asciiToIndex(c)];
            //} else if (columnFocus == 7) {
            //  o = list8.getListContents()[asciiToIndex(c)];
            //}
            if (o !== null && o !== undefined) {
              o.buttonSignal("hotkey");
            }
          }
        }
      }
    }
    
    List {
      id:list1
      uid:"1"
      border.width:1;
      border.color:"#202020";
      focused: columnFocus == 0;
      width:225;
    }
    
    List {
      id:list2
      uid:"2"
      anchors.left: list1.right;
      anchors.leftMargin: 5;
      border.width:1;
      border.color:"#202020";
      focused: columnFocus == 1;
      width:225;
    }
    
    List {
      id:list3
      uid:"3"
      anchors.left: list2.right;
      anchors.leftMargin: 5;
      border.width:1;
      border.color:"#202020";
      focused: columnFocus == 2;
      width:225;
    }
    
    List {
      id:list4
      uid:"4"
      anchors.left: list3.right;
      anchors.leftMargin: 5;
      border.width:1;
      border.color:"#202020";
      focused: columnFocus == 3;
      width:225;
    }
    
    
    //List {
    //  id:list5
    //  uid:"5"
    //  anchors.left: list4.right;
    //  anchors.leftMargin: 5;
    //  border.width:1;
    //  border.color:"#202020";
    //  focused: columnFocus == 4;
    //}
    //
    //
    //List {
    //  id:list6
    //  uid:"6"
    //  anchors.left: list5.right;
    //  anchors.leftMargin: 5;
    //  border.width:1;
    //  border.color:"#202020";
    //  focused: columnFocus == 5;
    //}
    //
    //
    //List {
    //  id:list7
    //  uid:"7"
    //  anchors.left: list6.right;
    //  anchors.leftMargin: 5;
    //  border.width:1;
    //  border.color:"#202020";
    //  focused: columnFocus == 6;
    //}
    //
    //
    //List {
    //  id:list8
    //  uid:"8"
    //  anchors.left: list7.right;
    //  anchors.leftMargin: 5;
    //  width:150
    //  border.width:1;
    //  border.color:"#202020";
    //  focused: columnFocus == 7;
    //}
    
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
    
    function getListContents(i) {
      if (i == 0) {
        return list1.getListContents();
      } else if (i == 1) {
        return list2.getListContents();
      } else if (i == 2) {
        return list3.getListContents();
      } else if (i == 3) {
        return list4.getListContents();
      }// else if (i == 4) {
      //  return list5.getListContents();
      //} else if (i == 5) {
      //  return list6.getListContents();
      //} else if (i == 6) {
      //  return list7.getListContents();
      //} else if (i == 7) {
      //  return list8.getListContents();
      //}
    }
    
    function randomColor() {
      return '#'+('000000'+(Math.random()*0xFFFFFF<<0).toString(16)).slice(-6);
    }
    
    function addElements() {
      list1.addElement();
      list2.addElement();
      list3.addElement();
      list4.addElement();
      //list5.addElement();
      //list6.addElement();
      //list7.addElement();
      //list8.addElement();
    }
    
    function change(c) {
      list1.change(c);
    }
}