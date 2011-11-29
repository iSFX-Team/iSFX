import QtQuick 1.0

Rectangle {
    id: progress
    property string state: "";
    property double percent: 0;
    property int min_width: 0;
    property int zero_width: 0;
    property int max_width: parent.width;
    width: (percent != 0 ? min_width+(max_width-min_width)*Math.min(1.0, Math.abs(percent/100)) : zero_width );
    //width: max_width*Math.min(1.0, Math.abs(percent/100));
    //onPercentChanged: {
    //  if (percent != 0)
    //    width = ;
    //  else
    //    width = zero_width;
    //}
    
    //states: [
    //  State {
    //    name: "loading";
    //    when: state == 1
    //    PropertyChanges { target: progress; 
    //      color: "orange";
    //      percent: 100;
    //    }
    //    PropertyChanges { target: load_animation; running: true; }
    //  },
    //  State {
    //    name: "stopped";
    //    when: state == 2
    //    PropertyChanges { target: progress; 
    //      color: "red";
    //      percent: 100;
    //    }
    //    PropertyChanges { target: progress; state: true; }
    //  }
    //]
    //
    //SequentialAnimation {
    //  id:load_animation
    //  PropertyAnimation { target: progress; property: "opacity"; to: 0.2; easing.type: Easing.Linear; duration: 500 }
    //  //SequentialAnimation {
    //  //  id:inner_loop
    //  //  loops: Animation.Infinite
    //  //  PropertyAnimation{ target: progress; property: "opacity";  from: 0.2; to: 0.3; easing.type: Easing.Linear; duration: 500}
    //  //  PropertyAnimation{ target: progress; property: "opacity";  from: 0.3; to: 0.2; easing.type: Easing.Linear; duration: 500}
    //  //}
    //}
    //
    //transitions: [
    //  Transition {
    //    from: "loading";
    //    SequentialAnimation {
    //      ParallelAnimation {
    //        PropertyAnimation { target: progress; property: "opacity"; to: 0.8; easing.type: Easing.Linear; duration: 200 }
    //        PropertyAnimation { target: progress; property: "color"; from: "orange"; to: "green"; easing.type: Easing.Linear; duration: 200 }
    //      }
    //      ParallelAnimation {
    //        PropertyAnimation { target: progress; property: "opacity"; to: 0.0; easing.type: Easing.Linear; duration: 750 }
    //        PropertyAnimation { target: progress; property: "percent"; from: 100; to: 0; easing.type: Easing.Linear; duration: 750 }
    //      }
    //      PropertyAction { target: progress; property:"opacity"; value: 0.30; }
    //    }
    //  },
    //  Transition {
    //    from: "stopped";
    //    SequentialAnimation {
    //      ParallelAnimation {
    //        PropertyAnimation { target: progress; property: "opacity"; to: 0.8; easing.type: Easing.Linear; duration: 100 }
    //        PropertyAnimation { target: progress; property: "color"; from: "green"; to: "red"; easing.type: Easing.Linear; duration: 100 }
    //      }
    //      ParallelAnimation {
    //        PropertyAnimation { target: progress; property: "opacity"; to: 0.0; easing.type: Easing.Linear; duration: 750 }
    //        PropertyAnimation { target: progress; property: "percent"; from: 100; to: 0; easing.type: Easing.Linear; duration: 750 }
    //      }
    //      PropertyAction { target: progress; property:"opacity"; value: 0.30; }
    //      PropertyAction { target: progress; property:"color"; value: "green"; }
    //    }
    //  }
    //]
}