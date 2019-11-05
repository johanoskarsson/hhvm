<?hh // strict

class Superclass {}
class Subclass extends Superclass {}

xhp class foo {
  attribute
    Superclass mysuperclass,
    Subclass mysubclass;
}

function main(): void {
  // type error
  $x = <foo mysubclass={new Superclass()} />;
}
