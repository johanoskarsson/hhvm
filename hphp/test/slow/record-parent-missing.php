<?hh

function foo() {
  $count = apc_fetch('rec-parent-count');
  if ($count === false) $count = 0;
  if ($count >= 2) return;

  if ($count == 0) {
    require 'record-parent-1.inc';
  }

  require 'record-child.inc';

  $a = D['y' => 42];
  var_dump($a['x']);
  var_dump($a['y']);

  $count++;
  apc_store('rec-parent-count', $count);
}

<<__EntryPoint>>
function main() {
  foo();
}
