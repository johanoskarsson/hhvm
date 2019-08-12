//// defn.php
<?hh // strict

namespace foo;

class :bar {}
class :something {}

//// usage.php
<?hh // strict

use foo\{:bar, :something};

function test(): void {
  <bar></bar>;
  <something></something>;
}
