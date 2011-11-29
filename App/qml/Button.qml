import QtQuick 1.0
//import "Root.js" as Root

Item {
  id:staticButton
  property alias buttonText: textItem.text
  property alias textColor: textItem.color
  property alias buttonColor: button.color
  property alias radius: button.radius
  signal buttonClicked
  width:  (textItem.paintedWidth + 10) >> 0 //the 10 here adds a buffer so the rounded corners don't cut off any of the lettering
  height: textItem.paintedHeight
  property variant borderColor: "#000000"
  
	 Rectangle {
		id: button
		width: parent.width; height: parent.height
		color: "#fff3f3f3"
		smooth: true
		radius:3
		border.color: staticButton.borderColor
		border.width:1
		
		MouseArea {
		  id: mouse
		  hoverEnabled: true
		  anchors.fill: parent
		  onClicked: buttonClicked()
		  onEntered: {
		    //swap the box/text colors
		    var color1 = parent.color;
		    var color2 = textItem.color;
		    textItem.color = color1;
		    parent.color = color2;
        //Root.set_cursor(staticButton, "pointing hand");
		  }
		  onExited: {
		    //swap the box/text colors back to normal
		    var color1 = parent.color;
		    var color2 = textItem.color;
		    textItem.color = color1;
		    parent.color = color2;
        //Root.set_cursor(staticButton, "pointing hand");
		  }
		}
		Text{
		  id: textItem
		  font.pointSize:13
		  font.bold:true
		  anchors.horizontalCenter: parent.horizontalCenter
		  anchors.verticalCenter: parent.verticalCenter
		  horizontalAlignment: Text.Center
		}
	 }
 }