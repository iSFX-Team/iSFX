// Each file that imports this one has its own version of these functions and variables
var contents = new Array();

function length() { return contents.length; }

function append(o) {
  o.index = contents.length
  contents[contents.length] = o;
}

function insert(i, o) {
  for(var j = contents.length; j > i; j--) {
    contents[j] = contents[j-1];
    contents[j].index = j;
    //console.log("moved " + (j-1) + " to " + j)
  }
  //console.log("inserted at " + i)
  contents[i] = o;
}

function swap(i, j) {
  var o = contents[i];
  contents[i] = contents[j];
  contents[i].index = i;
  contents[j] = o;
  contents[j].index = j;
}

function checked_swap(i,j) {
  if (i == j) return false;
  if (i < 0) i = 0;
  else if (i >= contents.length) i = contents.length-1;
  if (j < 0) j = 0;
  else if (j >= contents.length) j = contents.length-1;
  
  var o = contents[i];
  contents[i] = contents[j];
  contents[i].index = i;
  contents[j] = o;
  contents[j].index = j;
  return true;
}

function checked_move(i,j) {
  if (i == j) return false;
  if (i < 0) i = 0;
  else if (i >= contents.length) i = contents.length-1;
  if (j < 0) j = 0;
  else if (j >= contents.length) j = contents.length-1;

  var o = contents[i];
  for(var v = i; v < j; v++) {
    contents[v] = contents[v+1];
    contents[v].index = v;
  }
  for(var v = i; v > j; v--) {
    contents[v] = contents[v-1];
    contents[v].index = v;
  }
  contents[j] = o;
  contents[j].index = j;
  return true;
}

function replace(i, o) {
  contents[i] = o;
  contents[i].index = i;
}

function remove(i) {
  for(var j = i; j < contents.length-1; j++) {
    contents[j] = contents[j+1];
    contents[j].index = j;
    //console.log("moved " + (j+1) + " to " + j)
  }
  contents.length--;
}

(function unit_test() {
  append(1);
  console.log(contents.length)
  append(2);
  console.log(contents.length)
  append(3);
  console.log(contents.length)
  assert(contents[0] == 1, "failed at 1")
  assert(contents[1] == 2, "failed at 2")
  assert(contents[2] == 3, "failed at 3")
  insert(1,'a');
  console.log(contents.length)
  assert(contents[0] == 1, "failed at 4")
  assert(contents[1] == 'a', "failed at 5")
  assert(contents[2] == 2, "failed at 6")
  assert(contents[3] == 3, "failed at 7")
  replace(2, "foo")
  console.log(contents.length)
  assert(contents[2] == "foo", "failed at 8")
  remove(2);
  console.log(contents.length)
  assert(contents[0] == 1, "failed at 9")
  assert(contents[1] == 'a', "failed at 10")
  assert(contents[2] == 3, "failed at 11")
  console.log(contents)
  remove(0);
  console.log(contents.length)
  assert(contents[0] == 'a', "failed at 12")
  assert(contents[1] == 3, "failed at 13")
  remove(1);
  console.log(contents.length)
  assert(contents[0] == 'a', "failed at 14")
  console.log("run_test complete")
})

function assert(b, m) {
  if (!b) console.log(m);
}