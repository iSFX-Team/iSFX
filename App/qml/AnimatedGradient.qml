import QtQuick 1.0

Rectangle {
  property variant colors;
  property int index;
  property int pauseDuration;
  property int animationLength;
  property double startPercentage;
  
  gradient: Gradient {
    GradientStop { position: 0.0; 
      color: colors[index];
      Behavior on color {
        SequentialAnimation {
          PauseAnimation { duration: pauseDuration }
          ColorAnimation { duration: animationLength }
        }
      }
    }
    GradientStop { position: startPercentage; 
      color: colors[index];
      Behavior on color {
        SequentialAnimation {
          PauseAnimation { duration: pauseDuration }
          ColorAnimation { duration: animationLength }
        }
      }
    }
    GradientStop { position: 1.0;
      color: colors[index+1];
      Behavior on color {
        SequentialAnimation {
          PauseAnimation { duration: pauseDuration }
          ColorAnimation { duration: animationLength }
        }
      }
    }
  }
}