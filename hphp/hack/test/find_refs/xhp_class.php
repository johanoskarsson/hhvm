<?hh

xhp class xhp:foo-element {}

xhp class xhp:bar {
  attribute :xhp:foo-element;
  public function genFoo(
    :xhp:foo-element $e,
  ): :xhp:foo-element {
    return <xhp:foo-element />;
  }
}

function test(
  :xhp:foo-element $e,
): :xhp:foo-element {
  return <xhp:foo-element />;
}
