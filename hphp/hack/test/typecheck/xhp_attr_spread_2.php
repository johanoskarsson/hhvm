<?hh // strict
xhp class baz {}
xhp class foo {
  attribute string bar;
}

function spread(): void {
  // XHP should be allowed to spread
  <foo {...<baz />} bar="bar"></foo>;
}
