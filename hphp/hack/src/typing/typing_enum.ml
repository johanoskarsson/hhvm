(**
 * Copyright (c) 2015, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the "hack" directory of this source tree.
 *
 *)

(*****************************************************************************)
(* Module used to enforce that Enum subclasses are used reasonably.
 * Exports the Enum type as the type of all constants, checks that constants
 * have the proper type, and restricts what types can be used for enums.
 *)
(*****************************************************************************)
open Core_kernel
open Nast
open Typing_defs

module Phase = Typing_phase
module Cls = Decl_provider.Class

let member_type env member_ce =
  let lazy default_result = member_ce.ce_type in
  if Option.is_none member_ce.ce_xhp_attr then default_result
  else match default_result with
    | _, Tapply (enum_id, _)->
      (* XHP attribute type transform is necessary to account for
       * non-first class Enums:

       * attribute MyEnum x; // declaration: MyEnum
       * $this->:x;          // usage: MyEnumType
       *)
      let maybe_enum = Typing_env.get_class env (snd enum_id) in
      (match maybe_enum with
        | None -> default_result
        | Some tc ->
          (match Decl_enum.enum_kind (Cls.pos tc, Cls.name tc)
              (Cls.enum_type tc) (Cls.get_ancestor tc) with
                | None -> default_result
                | Some (_base, (_, enum_ty), _constraint) ->
                  let ty = (fst default_result), enum_ty in
                  ty
          ))
    | _ -> default_result

(* Check that a type is something that can be used as an array index
 * (int or string), blowing through typedefs to do it. Takes a function
 * to call to report the error if it isn't. *)
let check_valid_array_key_type f_fail ~allow_any:allow_any env p t =
  let rec check_valid_array_key_type env t =
    let ety_env = Phase.env_with_self env in
    let env, (r, t'), trail =
      Typing_tdef.force_expand_typedef ~ety_env env t in
    match t' with
    | Tprim (Tint | Tstring) -> env, None
    (* Enums have to be valid array keys *)
    | Tabstract (AKnewtype (id, _), _) when Typing_env.is_enum env id -> env, None
    | Terr | Tany when allow_any -> env, None
    | Tintersection tyl ->
      (* Ok if at least one element of the intersection is ok. *)
      let env, errors = List.fold_map tyl ~init:env ~f:check_valid_array_key_type in
      if List.exists errors ~f:Option.is_none
      then env, None
      else env, Option.value ~default:None (List.find errors ~f:Option.is_some)
    | Terr | Tany | Tnonnull | Tarraykind _ | Tprim _ | Toption _ | Tdynamic
      | Tvar _ | Tabstract _ | Tclass _ | Ttuple _ | Tanon _ | Tdestructure _
      | Tfun _ | Tunion _ | Tobject | Tshape _ ->
      env, Some (fun () -> f_fail p (Reason.to_pos r) (Typing_print.error env (r, t')) trail) in
  let env, err = check_valid_array_key_type env t in
  Option.iter err (fun f -> f ());
  env

let enum_check_const ty_exp env cc t =
  let p = fst cc.cc_id in
  (* Constants need to be subtypes of the enum type *)
  let env = Typing_ops.sub_type p Reason.URenum env t ty_exp
    Errors.constant_does_not_match_enum_type in
  (* Make sure the underlying type of the constant is an int
   * or a string. This matters because we need to only allow
   * int and string constants (since only they can be array
   * indexes). *)
  (* Need to allow Tany, since we might not have the types *)
  check_valid_array_key_type
    Errors.enum_constant_type_bad
    ~allow_any:true env p t

(* If a class is a subclass of Enum<T>, check that the types of all of
 * the constants are compatible with T.
 * Also make sure that T is either int, string, or mixed (or an
 * abstract type that is one of those under the hood), that all
 * constants are ints or strings when T is mixed, and that any type
 * hints are compatible with the type. *)
let enum_class_check env tc consts const_types =
  let enum_info_opt =
    Decl_enum.enum_kind (Cls.pos tc, Cls.name tc) (Cls.enum_type tc)
    (Cls.get_ancestor tc) in
  match enum_info_opt with
    | Some (ty_exp, _, ty_constraint) ->
        let ety_env = Phase.env_with_self env in
        let env, ty_exp = Phase.localize ~ety_env env ty_exp in
        let env, (r, ty_exp'), trail =
          Typing_tdef.force_expand_typedef ~ety_env env ty_exp in
        (match ty_exp' with
          (* We disallow first-class enums from being non-exact types, because
           * a switch on such an enum can lead to very unexpected results,
           * since switch uses == equality. *)
          | Tnonnull | Tprim Tarraykey when Cls.enum_type tc <> None ->
              Errors.enum_type_bad (Reason.to_pos r)
                (Typing_print.error env (r, ty_exp')) trail
          | Tnonnull when snd ty_exp <> Tnonnull ->
              Errors.enum_type_typedef_nonnull (Reason.to_pos r)
          | Tnonnull -> ()
          | Tprim Tint | Tprim Tstring | Tprim Tarraykey -> ()
          (* Allow enums in terms of other enums *)
          | Tabstract (AKnewtype (id, _), _) when Typing_env.is_enum env id -> ()
          (* Don't tell anyone, but we allow type params too, since there are
           * Enum subclasses that need to do that *)
          | Tabstract (AKgeneric _, _) -> ()
          | Terr | Tany | Tarraykind _ | Tprim _ | Toption _ | Tvar _
            | Tabstract (_, _) | Tclass _ | Ttuple _ | Tanon (_, _) | Tdestructure _
            | Tunion _ | Tintersection _ | Tobject | Tfun _ | Tshape _ | Tdynamic ->
              Errors.enum_type_bad (Reason.to_pos r)
                (Typing_print.error env (r, ty_exp')) trail);

        (* If a constraint was given, make sure that it is a subtype
           of arraykey, and that the base type is actually a subtype
           of it. *)
        let env = (match ty_constraint with
          | Some ty ->
             let env, ty = Phase.localize ~ety_env env ty in
             let ty_arraykey = (
               Reason.Rimplicit_upper_bound (Cls.pos tc, "arraykey"),
               Tprim Tarraykey
             ) in
             let env = Typing_ops.sub_type (Cls.pos tc) Reason.URenum_cstr env
               ty ty_arraykey Errors.enum_constraint_must_be_arraykey in
             Typing_ops.sub_type (Cls.pos tc) Reason.URenum_cstr env ty_exp ty
               Errors.enum_subtype_must_have_compatible_constraint
          | None -> env) in

        List.fold2_exn ~f:(enum_check_const ty_exp) ~init:env consts const_types

    | None -> env
