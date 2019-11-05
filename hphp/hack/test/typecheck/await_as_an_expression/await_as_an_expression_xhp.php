<?hh // strict

final xhp class my-xhp {
  attribute int x;
}

async function f(): Awaitable<void> {
  <my-xhp x={await async { return 42; }}>
    {await async { return null; }}
  </my-xhp>;
}
