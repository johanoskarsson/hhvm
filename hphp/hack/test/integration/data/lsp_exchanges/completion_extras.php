<?hh  //strict

/** :ab:cd:text docblock */
final xhp class ab:cd:text implements XHPChild {
  attribute string color, int width;
}

/** :ab:cd:alpha docblock */
final xhp class ab:cd:alpha implements XHPChild {
  attribute string name;
}

enum Elsa: string {
  Alonso = "hello";
  Bard = "world";
}
