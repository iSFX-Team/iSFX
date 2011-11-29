import QtQuick 1.1

Rectangle {
  id: edit_window
  
  property variant oldModelData;
  property variant modelData;
  property variant index;
  
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
      //modelData.waveformFileChanged.connect(load_waveform);
      //modelData.getWaveform();
    }
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
  
  color: "red"
  Row {
    height: parent.height;
    width: parent.width*3/4;
    Rectangle {
      height: parent.height;
      width: parent.width;
      Waveform {
        id: waveform
        x: 5
        waveformFilePath: (modelData !== undefined ? "../"+modelData.waveformFile : "");
        height: parent.height;
        width: parent.width-10;
        percent: (modelData !== undefined) ? modelData.percent : 0;
        onStartChanged: { if (modelData !== undefined) modelData.start = start*modelData.length; }
        onStopChanged: { if (modelData !== undefined) modelData.stop = stop*modelData.length; }
        onFadeInChanged: { if (modelData !== undefined) modelData.fadeIn = fadeIn*modelData.length; }
        onFadeOutChanged: { if (modelData !== undefined) modelData.fadeOut = fadeOut*modelData.length; }
      }
    }
    Column {
      width: parent.width/4;
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
          property double last_value: 0;
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
    }
  }
  
  states: [
      State {
        
        name: "active"; 
        when: edit_window.modelData !== undefined;
        
        PropertyChanges {
          target: edit_window
          color: "green"
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
      }
  ]
}