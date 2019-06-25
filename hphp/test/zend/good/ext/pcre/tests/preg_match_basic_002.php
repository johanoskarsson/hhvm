<?hh
/* Prototype  : int preg_match  ( string $pattern  , string $subject  [, array &$matches  [, int $flags  [, int $offset  ]]] )
 * Description: Perform a regular expression match
 * Source code: ext/pcre/php_pcre.c
 */
<<__EntryPoint>> function main(): void {
$string = "My\nName\nIs\nStrange";
preg_match("/M(.*)/", $string, &$matches);

var_dump($matches);
echo "===Done===";
}
