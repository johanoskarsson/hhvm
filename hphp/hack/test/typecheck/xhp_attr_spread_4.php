<?hh // strict

xhp class foo {
  attribute int name, int age;
}

xhp class bar {
  attribute string name;
}

function test(:foo $x): :bar {
  // Name is not compatible across the spread
  return <bar {...$x} />;
}
