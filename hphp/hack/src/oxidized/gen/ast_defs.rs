// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.
//
// @generated SignedSource<<3bf3dd58f5b19995a32f51d9f315172c>>
//
// To regenerate this file, run:
//   hphp/hack/src/oxidized/regen.sh

use ocamlrep_derive::OcamlRep;

use crate::pos;

pub use crate::shape_map;

pub use pos::Pos;

#[derive(Clone, Debug, OcamlRep)]
pub struct Id(pub Pos, pub String);

pub type Pstring = (Pos, String);

#[derive(Clone, Debug, OcamlRep)]
pub enum ShapeFieldName {
    SFlitInt(Pstring),
    SFlitStr(Pstring),
    SFclassConst(Id, Pstring),
}

#[derive(Clone, Copy, Debug, Eq, OcamlRep, PartialEq)]
pub enum Variance {
    Covariant,
    Contravariant,
    Invariant,
}

#[derive(Clone, Copy, Debug, Eq, OcamlRep, PartialEq)]
pub enum ConstraintKind {
    ConstraintAs,
    ConstraintEq,
    ConstraintSuper,
}

pub type Reified = bool;

#[derive(Clone, Copy, Debug, Eq, OcamlRep, PartialEq)]
pub enum ClassKind {
    Cabstract,
    Cnormal,
    Cxhp,
    Cinterface,
    Ctrait,
    Cenum,
}

#[derive(Clone, Copy, Debug, Eq, OcamlRep, PartialEq)]
pub enum ParamKind {
    Pinout,
}

#[derive(Clone, Copy, Debug, Eq, OcamlRep, PartialEq)]
pub enum OgNullFlavor {
    OGNullthrows,
    OGNullsafe,
}

#[derive(Clone, Copy, Debug, Eq, OcamlRep, PartialEq)]
pub enum FunKind {
    FSync,
    FAsync,
    FGenerator,
    FAsyncGenerator,
    FCoroutine,
}

#[derive(Clone, Debug, OcamlRep)]
pub enum Bop {
    Plus,
    Minus,
    Star,
    Slash,
    Eqeq,
    Eqeqeq,
    Starstar,
    Diff,
    Diff2,
    Ampamp,
    Barbar,
    LogXor,
    Lt,
    Lte,
    Gt,
    Gte,
    Dot,
    Amp,
    Bar,
    Ltlt,
    Gtgt,
    Percent,
    Xor,
    Cmp,
    QuestionQuestion,
    Eq(Option<Box<Bop>>),
}

#[derive(Clone, Copy, Debug, Eq, OcamlRep, PartialEq)]
pub enum Uop {
    Utild,
    Unot,
    Uplus,
    Uminus,
    Uincr,
    Udecr,
    Upincr,
    Updecr,
    Usilence,
}

#[derive(Clone, Copy, Debug, Eq, OcamlRep, PartialEq)]
pub enum FunDeclKind {
    FDeclAsync,
    FDeclSync,
    FDeclCoroutine,
}
