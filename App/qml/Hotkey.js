function fromIndex(i) {
  return indexToBill(i);
}

function toIndex(i) {
  return billToIndex(i);
}

function indexToBill(i) {
return "qwertyuiopasdfghjklzxcvbnm".charAt(i);
}
function billToIndex(k) {
return "qwertyuiopasdfghjklzxcvbnm".indexOf(k);
}

function indexToQwerty(i) {
  return "1234567890qwertyuiopasdfghjklzxcvbnm".charAt(i);
}
function qwertyToIndex(k) {
  return "1234567890qwertyuiopasdfghjklzxcvbnm".indexOf(k);
}
function indexToAscii(i) {
  return String.fromCharCode(i < 10 ? i+48 : i+87);
}
function asciiToIndex(k) {
  var i = k.charCodeAt(0);
  return (i < 58 ? i-48 : i-87);
}