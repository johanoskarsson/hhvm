<?hh // strict

xhp class foo {
  attribute string name;

  public async function genRender(): Awaitable<mixed> {
    // Another case of unsoundness: we pretend that `this` means the class
    // where the spread is written, even though it may be a :subfoo.
    return <bar {...$this} />;
  }
}

xhp class subfoo extends :foo {
  attribute string age;
}

xhp class bar {
  attribute int age;
}
