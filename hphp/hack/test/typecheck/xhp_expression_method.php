<?hh // partial
final xhp class x {
  public function foo(): string {
    return "";
  }
}


function test(): string {
  return <x></x>->foo();
}
