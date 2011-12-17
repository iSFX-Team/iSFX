import QtQuick 1.1
import "Theme.js" as Theme

Rectangle {
  id: edit_window
  
  property variant oldModelData;
  property variant modelData;
  property variant index;
  property int dummy: 1;
  property bool realtimeWaveform: false;
  
  Timer {
    id: waveDataTimer
    interval: 50;
    repeat: true;
    onTriggered: {
      dummy += 1
      waveform.waveformFilePath = "image://waveFactory/foo"+dummy
    }
  }
  
  onRealtimeWaveformChanged: {
    console.log("onRealtimeWaveformChanged")
    if (realtimeWaveform) {
      waveDataTimer.start();
      waveform.slidersVisible = false;
    } else {    
      waveform.slidersVisible = true;
      waveDataTimer.stop();
      if (modelData !== undefined) {
        waveform.waveformFilePath = "../"+modelData.waveformFile;
      } else {
        waveform.waveformFilePath = "../blank.svg";
      }
    }
  }
  
  onModelDataChanged: {
    if (oldModelData !== undefined) {
      oldModelData.startChanged.disconnect(update_waveform);
      oldModelData.stopChanged.disconnect(update_waveform);
      oldModelData.fadeInChanged.disconnect(update_waveform);
      oldModelData.fadeOutChanged.disconnect(update_waveform);
      //oldModelData.waveformFileChanged.disconnect(load_waveform);
    }
    oldModelData = modelData;
    update_waveform();
    if (modelData !== undefined) {
      modelData.startChanged.connect(update_waveform);
      modelData.stopChanged.connect(update_waveform);
      modelData.fadeInChanged.connect(update_waveform);
      modelData.fadeOutChanged.connect(update_waveform);
      //edit_tab.slider.set_value(modelData.masterVolume)
      //modelData.waveformFileChanged.connect(load_waveform);
      if (!realtimeWaveform) {
        waveDataTimer.stop();
        waveform.waveformFilePath = "../"+modelData.waveformFile;
        waveform.slidersVisible = true;
      }
    }// else {
    //  console.log("STARTING TIMER STARTING TIMER STARTING TIMER")
    //  waveDataTimer.start();
    //}
  }
  
  function update_waveform() {
    if (modelData !== undefined) {
      waveform.set_waveform( modelData.start*1.0/modelData.length,
                             modelData.stop*1.0/modelData.length,
                             modelData.fadeIn*1.0/modelData.length,
                             modelData.fadeOut*1.0/modelData.length );
    } else {
      waveform.set_waveform( 0, 1.00, 0.025, 0.025 );
    }
  }
  
  function load_waveform() {
    if (modelData !== undefined) {
      waveform.waveformFilePath = modelData.waveformFile;
    }
  }
  
  function do_promote() {
    if (modelData !== undefined) {
      promote(modelData.index)
    }
  }
  
  function do_demote() {
    if (modelData !== undefined) {
      demote(modelData.index)
    }
  }
  
  color: Theme.ft_ed_bkg_inactive
  
  height: parent.height;
  width: parent.width;
  Rectangle {
    height: parent.height;
    width: parent.width-tabs.width-tabsBorder.width;
    Waveform {
      id: waveform
      x: 5
      //cache: true;
      //waveformFilePath: (modelData !== undefined ? "../"+modelData.waveformFile : "");
      height: parent.height;
      width: parent.width-10;
      percent: (modelData !== undefined) ? modelData.percent : 0;
      onStartChanged: { if (modelData !== undefined) modelData.start = start*modelData.length; }
      onStopChanged: { if (modelData !== undefined) modelData.stop = stop*modelData.length; }
      onFadeInChanged: { if (modelData !== undefined) modelData.fadeIn = fadeIn*modelData.length; }
      onFadeOutChanged: { if (modelData !== undefined) modelData.fadeOut = fadeOut*modelData.length; }
    }
  }
  Rectangle {
    id: tabsBorder
    anchors.right: tabs.left;
    width: 1
    height: parent.height;
    color: "black"
  }
  Tabs {
    id: tabs
    anchors.right: parent.right;
    tabColor: "darkgray"
    activeTabColor: "orange"
    tabBackground: "gray"
    width: 240;
    height: parent.height
    model: [
      edit_tab, 
      timer_tab,
      options_tab,
      about_tab
    ]
  }
  
  
  /*
   *  Timer Tab
   *
   */
  Component {
    id: timer_tab
    
    Item {
      id: timer_tab_container
      
      
      
      anchors.fill: parent;
      property variant tab;
      
      Binding {
        when: tab !== undefined
        target: tab
        property: "text"
        value: (stopwatch.running && !visible ? "Timer: " + timer_text.text : "Timer")
      }
      
      Stopwatch {
        id: stopwatch;
      }
      
      Column {
        x: parent.width/2-width/2
        spacing: 3
        Text {
          id: timer_text;
          property int seconds: 0;
          property bool paused: false;
          text: ('00'+(stopwatch.diff/60).toFixed(0)).substr(-2) + ":" + ('00'+(stopwatch.diff%60).toFixed(0)).substr(-2);
          font.pixelSize: 40
          color: "black";
        }
        Button {
        	buttonText: "Start"
        	borderColor: "black"
        	buttonColor: "white"
        	onButtonClicked: {
        	  if (!stopwatch.running) {
        	    stopwatch.start();
      	    }
        	}
        }
        Button {
        	buttonText: "Stop"
        	borderColor: "black"
        	buttonColor: "white"
        	onButtonClicked: {
        	  stopwatch.stop();
        	}
        }
        Button {
        	buttonText: "Pause"
        	borderColor: "black"
        	buttonColor: "white"
        	onButtonClicked: {
        	  stopwatch.pause();
        	}
        }
        Button {
        	buttonText: "Reset"
        	borderColor: "black"
        	buttonColor: "white"
        	onButtonClicked: {
        	  stopwatch.reset();
        	}
        }
      }
    }
  }
  
  Component {
    id: options_tab;
    Item {
      anchors.fill: parent;
      
      property variant tab;
      onTabChanged: {
        tab.text = "Options"
      }
      
      Checkbox {
        text: "realtime waveform"
        onCheckedChanged: {
          edit_window.realtimeWaveform = checked;
        }
      }
    }
  }
  
  /*
   *  About Tab
   *
   */
  Component {
    id: about_tab
    Item {
      anchors.fill: parent;
      
      property variant tab;
      onTabChanged: {
        tab.text = "About"
      }
      
      Column {
        anchors.fill: parent;
        Text {
          text: "About iSFX..."
        }
        Hyperlink {
          text: "Github Page"
          url: "https://github.com/iSFX-Team/iSFX"
        }
      }
    }
  }
  
  /*
   *  Edit Tab
   *
   */
  Component {
    id: edit_tab
    
    Item {
      property alias slider: slider;
      anchors.fill: parent;
      
      property variant tab;
      onTabChanged: {
        tab.text = "Edit"
      }
      
      //color: "lightgray"
      Column {
        width: parent.width;
        height: parent.height;
        spacing: 5;
        
        Text {
          id: actual_volume_text
          text: "Actual Volume: n/a"
        }
        Row {
          width: parent.width;
          spacing: 5;
          Text { id:name_label; text: "Name:" }
          StringInput {
            id: name_input;
            width: parent.width-name_label.paintedWidth-15;
            text: "[name]";
            onAccepted: { if (modelData !== undefined) { modelData.name = text; } }
          }
        }
        Row {
          spacing: 5;
          width: parent.width;
          Text { id: volumelabel; text: "Volume:" }
          HorizontalSlider {
            id: slider;
            height: 8;
            y: parent.height/2-height/2
            width: parent.width*0.5;
            property variant modelData: edit_window.modelData;
            onModelDataChanged: {
              if (modelData !== undefined) {
                set_value(modelData.masterVolume);
              } else {
                set_value(0.0);
              }
            }
            onValueChanged: { if (modelData !== undefined) { modelData.masterVolume = value; } }
          }
          DoubleInput {
            id: volume_edit_box;
            min: 0; max: 1;
            onAccepted: slider.set_value(value);
          }
        }
        Row {
          spacing: 5;
          Text { text: "Start:" }
          DoubleInput {
            id: start_input;
            min: 0;
            onAccepted: { if (modelData !== undefined) { modelData.start = value*1000; } }
          }
          Text { text: "Stop:" }
          DoubleInput {
            id: stop_input;
            min: 0;
            onAccepted: { if (modelData !== undefined) { modelData.stop = value*1000; } }
          }
        }
        Row {
          spacing: 5;
          Text { text: "FadeIn:" }
          DoubleInput {
            id: fadein_input;
            min: 0;
            onAccepted: { if (modelData !== undefined) { modelData.fadeIn = value*1000; } }
          }
          Text { text: "FadeOut:" }
          DoubleInput {
            id: fadeout_input;
            min: 0;
            onAccepted: { if (modelData !== undefined) { modelData.fadeOut = value*1000; } }
          }
        }
        Row {
          spacing: 5;
          Text { text: "FadeStop:" }
          DoubleInput {
            id: fadestop_input;
            min: 0;
            onAccepted: { if (modelData !== undefined) { modelData.fadeStop = value*1000; } }
          }
        }
        Row {
          Checkbox {
            text: "highpass"
            onCheckedChanged: {
              if (modelData !== undefined) {
                modelData.highPassFilter = checked;
              }
            }
          }
          Checkbox {
            text: "lowpass"
            onCheckedChanged: {
              if (modelData !== undefined) {
                modelData.lowPassFilter = checked;
              }
            }
          }
        }
      }
      
      states: [
          State {
      
            name: "active"; 
            when: edit_window.modelData !== undefined;
      
            PropertyChanges {
              target: edit_window
              color: Theme.ft_ed_bkg
            }
            PropertyChanges { 
              target: start_input;
              text: (modelData.start/1000).toFixed(3);
            }
            PropertyChanges { 
              target: stop_input;
              text: (modelData.stop/1000).toFixed(3);
            }
            PropertyChanges { 
              target: fadein_input;
              text: (modelData.fadeIn/1000).toFixed(3);
            }
            PropertyChanges { 
              target: fadeout_input;
              text: (modelData.fadeOut/1000).toFixed(3);
            }
            PropertyChanges { 
              target: fadestop_input;
              text: (modelData.fadeStop/1000).toFixed(3);
            }
            PropertyChanges { 
              target: name_input;
              text: modelData.name;
            }
            PropertyChanges { 
              target: actual_volume_text;
              text: "Actual Volume: "+(modelData.actualVolume).toFixed(3);
            }
            PropertyChanges { 
              target: volume_edit_box;
              text: (modelData.masterVolume).toFixed(3);
            }
          }
      ]
    }
  }
}