<?hh

class test {
    function __unset(&$name) { }
}
<<__EntryPoint>> function main(): void {
$t = new test;
$name = "prop";

var_dump($t->$name);

echo "Done\n";
}
