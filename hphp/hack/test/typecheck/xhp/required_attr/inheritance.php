<?hh // strict

xhp class base { attribute int a @required; }
xhp class derived extends :base {}
function bar3(): :derived {
  return <derived />;
}
