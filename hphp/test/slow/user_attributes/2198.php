<?hh

<<A(1),B('foo',array(42,73))>>
class C {
  <<A(2),B('bar',array(43,74))>>
  function f() {
}
}

<<A(3),B('bar',array(44,75)), C('concat'.'enate')>>
function f() {
}

<<__EntryPoint>>
function main_2198() {
$rc = new ReflectionClass('C');
$attrs = $rc->getAttributes();
ksort(&$attrs);
var_dump($attrs);
$rm = $rc->getMethod('f');
$attrs = $rm->getAttributes();
ksort(&$attrs);
var_dump($attrs);
$rf = new ReflectionFunction('f');
$attrs = $rf->getAttributes();
ksort(&$attrs);
var_dump($attrs);
}
