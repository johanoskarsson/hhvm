<?hh // strict

xhp class foo {
  attribute string name;
}

xhp class subfoo extends :foo {
  attribute string age;
}

xhp class bar {
  attribute int age;
}

function get_foo(): :foo {
  return <subfoo />;
}

function test(): void {
  $foo = get_foo();
  // No errors, even though we are copying string :age into int :age since we
  // have :subfoo at runtime
  <bar {...$foo} />;
}
