<?hh // strict
// Copyright 2004-present Facebook. All Rights Reserved.

xhp class a { attribute int a @required;}
xhp class b { attribute int a @lateinit;}
xhp class c { attribute int a;}
xhp class d {
  attribute
    int a @required,
    int b @lateinit,
    int c;
}
xhp class e { attribute :a;}
xhp class f { attribute :b;}

function bar(): void {
  // No error
  $a = <a a={1} />;
  $a = <b />;
  $a = <c />;
  $a = <f />;

  // Error
  $a = <a />;
  $a = <d />;
  $a = <e />;
}
