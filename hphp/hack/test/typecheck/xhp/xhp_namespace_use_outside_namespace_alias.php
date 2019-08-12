//// defn.php
<?hh // strict

namespace foo;

class :bar {}

//// usage.php
<?hh // strict

use foo as fubar;

function test(): void {
  <fubar:bar></fubar:bar>;
  <:foo:bar></:foo:bar>;
}
