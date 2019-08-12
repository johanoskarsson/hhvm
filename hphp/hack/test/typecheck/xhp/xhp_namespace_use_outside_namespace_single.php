//// defn.php
<?hh // strict

namespace foo;

class :bar {}

//// usage.php
<?hh // strict

use foo\:bar;

function test(): void {
  <bar></bar>;
}
