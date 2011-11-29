/*

In order to bring the window to the front some python code needs to be called
"view.raise_()". This code finds the root node and emits the raise_ signal,
which in turn should call a python function which raises the window to the
front.

This should be used if an event should bring the window to the front.

*/

.pragma library

var root;
function get_root(e) {
  if (root === undefined || root === null) {
    while (e !== null && e !== undefined) {
      root = e;
      e = e.parent;
    }
  }
  return root;
}

function set_root(e) {
  if (root === undefined || root === null) {
    while (e !== null && e !== undefined) {
      root = e;
      e = e.parent;
    }
  }
}

function raise() {
  if (root === undefined || root === null) {
    set_root(e);
  }
  root.raise_();
}

function set_cursor(e, s) {
  if (root === undefined || root === null) {
    set_root(e);
  }
  root.set_cursor(e, s);
}