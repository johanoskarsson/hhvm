<?hh // strict

final xhp class m:a extends :m:xui:block {
  attribute :m:xui:div;
}

final xhp class m:b extends :m:xui:block {
  attribute LongNameCollectionCausingLineBreak my-verbosely-named-collection @required;
}

final xhp class m:c extends :m:xui:block {
  attribute :m:xui:div, HandbagCollection collection @required;
}

final xhp class m:d extends :m:xui:block {
  attribute :m:xui:div, HandbagCollection collection @required, AccessorySet accessories;
}
