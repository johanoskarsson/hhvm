<?hh

class A {
 public static $a = array('a', 'b');
 public static function test() {
 self::$a[] = 'c';
 var_dump(self::$a);
}
 }

 <<__EntryPoint>>
function main_1245() {
A::test();
}
