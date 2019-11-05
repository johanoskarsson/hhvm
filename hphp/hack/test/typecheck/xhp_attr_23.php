<?hh // strict

xhp class foo {}

function main(): void {
  $x = <foo bar="baz" />; // undefined attribute
}
