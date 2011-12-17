import QtQuick 1.1

Item {
  property int diff:0;
  
  Timer {
    id: timer;
    property bool stopped: true;
    
    interval: 1000;
    triggeredOnStart: true;
    repeat: true;
    onTriggered: {
      diff += 1
    }
  }
    
  function start() {
    if (timer.stopped) {
      diff = 0;
    }
    timer.stopped = false;
    timer.start();
  }
  
  function stop() {
    timer.stopped = true;
    timer.stop();
  }
  
  function reset() {x
    diff = 0;
  }
  
  function pause() {
    timer.stop();
  }
}