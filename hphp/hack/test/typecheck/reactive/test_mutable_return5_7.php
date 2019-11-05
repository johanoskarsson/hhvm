<?hh // strict

class A {
  <<__Rx, __MutableReturn>>
  public function f8(): :cls {
    // OK
    return <cls />;
  }
}

xhp class cls {
  public ?int $x;
}
