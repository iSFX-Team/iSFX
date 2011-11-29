import QtQuick 1.0
import "../qml/Components"

Item {
		id: checkbox
		
		property bool isChecked:false;
		signal checked(bool checked);
		
		width: checkBoxLabel.paintedWidth + checkBoxBG.width;
		height: 18;
		x:50; // for my testing
		y: 50; // for my testing
		
		anchors.leftMargin : 5 // note this is setting the default margins
		anchors.rightMargin: 5 // it wont do anything unless the user
		anchors.topMargin: 5   // uses Checkbox { anchors.right: something.left; }
		anchors.bottomMargin: 5

		Rectangle{
			id: checkBoxBG
			width: 10
			height: 10
			color: "#FFFFFF"
			border.color: "black" // i changed this to black because it was refering to a variable that didn't exist
			border.width: 1
			Text{
				id: checkmark
				y: -3
				text: isChecked ? "\u2713" : ""
			}
		}
		Text{
			id: checkBoxLabel
			text: "Label"
			anchors.left: checkBoxBG.right
			anchors.leftMargin: 5
			anchors.verticalCenter: checkBoxBG.verticalCenter
		}
		MouseArea {
		  id: mouseArea // don't use mouse b/c mouse is a variable passed into onClicked
		  anchors.fill: parent;
		  onClicked: {
  			isChecked = !isChecked; 
  			checked(isChecked);
		  }
		}
		
	}