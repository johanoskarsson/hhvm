<?hh // strict
// Copyright 2004-present Facebook. All Rights Reserved.

xhp class a1             { attribute int a @required; }
xhp class b1 extends :a1 { attribute int a @required; }
xhp class c1 extends :a1 { attribute int a @lateinit;}
xhp class d1 extends :a1 { attribute int a; }

xhp class a2             { attribute int a @lateinit; }
xhp class b2 extends :a2 { attribute int a @required; }
xhp class c2 extends :a2 { attribute int a @lateinit; }
xhp class d2 extends :a2 { attribute int a; }

xhp class a3             { attribute int a; }
xhp class b3 extends :a3 { attribute int a @required; }
xhp class c3 extends :a3 { attribute int a @lateinit; }
xhp class d3 extends :a3 { attribute int a; }
