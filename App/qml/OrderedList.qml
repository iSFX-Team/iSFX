import QtQuick 1.1
import "Theme.js" as Theme

Rectangle {
  id: container
  
  property int count;
  
  // Sounds are positioned based on this placeholder
  // this is because we want the ability to drag the
  // Sounds around. Flow or any other positioning
  // Component does not allow the changeing of x or y
  // values. So, the placeholders are created, the
  // view is created using the delegate, and the
  // delegate should Bind to the placeholder's x and y
  // position and should Bind the placeholders width
  // and height to that of the delegate.
  property Component delegate;
  property Component placeholder: Item {
    property variant viewData;
    property int index: -1;
  }
  
  property QtObject viewData: QtObject {
    id: viewDataObject;
    property int insertAbove: -1;
    property variant placeholders;
    property int columnPadding: 1;
    property int rowPadding: 1;
    property int height: grid.childrenRect.height;
    property int focusedColumn: 0;
  }
  
  Item {
    id: visual_layer
    anchors.fill: parent;
    
    Flow {
      id: grid
      anchors.fill: parent;
      flow: Flow.TopToBottom
    }
  }
  
  Component.onCompleted: {
    var list = new Array();
    for(var i = 0; i < count; i++) {
      var p = placeholder.createObject(grid, {index: i, viewData: viewDataObject})
      list.push(p);
      var d = delegate.createObject(visual_layer, {index: i, viewData: viewDataObject})
    }
    viewDataObject.placeholders = list;
  }
}