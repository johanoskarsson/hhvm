<?hh // strict

function f(inout int $i): void {}

xhp class x:foo {
  attribute int attr @required;

  public function test(): void {
    f(inout $this->:attr);
  }
}
