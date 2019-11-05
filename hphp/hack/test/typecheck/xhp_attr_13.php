<?hh // partial

xhp class something extends xhpclass {
  attribute Foo x @required;

  public function render() {
    takes_string($this->:x);
  }
}

function takes_string(string $s) {}

class Foo extends Enum<int> {}

xhp class xhpclass {}

class Enum<T> {}
