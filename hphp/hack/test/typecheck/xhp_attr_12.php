<?hh // partial
xhp class foo {
  attribute :bar;
}
xhp class bar {
  attribute array blah = array();
}
function test(:foo $obj): int {
  return $obj->:blah;
}
