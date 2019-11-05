<?hh // strict

xhp class my_foo<T> {
  attribute vec<T> my-vec;
}

xhp class my_bar {
  attribute vec<int> my-vec;
}

function test(): void {
  // This is legal, $f is :my-foo<int>
  $f = <my_foo my-vec={vec[1,2,3]} />;
  <my_bar {...$f} />;

  // This is incorrect through the generic
  $f2 = <my_foo my-vec={vec['foo', 'bar', 'baz']} />;
  <my_bar {...$f2} />;
}
