<?hh // strict
// Copyright 2004-present Facebook. All Rights Reserved.

function dyn(): dynamic { return 4; }

type X = int;
newtype Y = int;

// TODO(T45690473): being overly conservative in this case
function f(): X {
  return dyn(); // error
}

function g(): Y {
  return dyn(); // error
}
