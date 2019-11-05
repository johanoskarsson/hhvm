<?hh
// Copyright 2004-present Facebook. All Rights Reserved.


final xhp class format-xhp:part1 {
   children (:fb:fundingsource:data-table:header,
     :fb:fundingsource:data-table:row*);
}

final xhp class format-xhp:part2 {
   children (:fb:fundingsource:data-table:header // hello
     , :fb:fundingsource:data-table:row*);
}

final xhp class format-xhp:part3 {
  children ((%image, (pcdata | %phrase)?) | ((pcdata | %phrase), %image?));
}

final xhp class format-xhp:part4 {
  children empty;
}

final xhp class format-xhp:part5 {
  children (:fb:social-pressure:friend-list-item)*;
}
