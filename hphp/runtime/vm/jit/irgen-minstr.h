/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#ifndef incl_HPHP_JIT_IRGEN_MINSTR_H_
#define incl_HPHP_JIT_IRGEN_MINSTR_H_

#include "hphp/runtime/base/static-string-table.h"

#include "hphp/runtime/vm/jit/extra-data.h"
#include "hphp/runtime/vm/jit/guard-constraint.h"
#include "hphp/runtime/vm/jit/ir-builder.h"
#include "hphp/runtime/vm/jit/irgen.h"
#include "hphp/runtime/vm/jit/irgen-exit.h"
#include "hphp/runtime/vm/jit/irgen-internal.h"
#include "hphp/runtime/vm/jit/irgen-state.h"
#include "hphp/runtime/vm/jit/array-access-profile.h"
#include "hphp/runtime/vm/jit/ssa-tmp.h"
#include "hphp/runtime/vm/jit/target-profile.h"
#include "hphp/runtime/vm/jit/type-profile.h"

namespace HPHP { namespace jit { namespace irgen {

///////////////////////////////////////////////////////////////////////////////

/*
 * Use profiling data from an ArrayAccessProfile to conditionally optimize
 * the array access represented by `generic' as `direct'.
 *
 * For profiling translations, this generates the profiling instructions, then
 * falls back to `generic'.  If we can perform the likely-offset optimization,
 * this branches on a CheckMixedArrayOffset/CheckDictOffset/CheckKeysetOffset
 * to either `direct' or `generic'; otherwise, we fall back to `generic'.
 *
 * The callback function signatures should be:
 *
 *    SSATmp* direct(SSATmp* key, uint32_t pos);
 *    SSATmp* generic(SSATmp* key);
 */
template<class DirectFn, class GenericFn>
SSATmp* profiledArrayAccess(IRGS& env, SSATmp* arr, SSATmp* key,
                            DirectFn direct, GenericFn generic,
                            bool cow_check = false) {
  const bool is_dict = arr->isA(TDict);
  const bool is_keyset = arr->isA(TKeyset);
  assertx(is_dict || is_keyset || arr->isA(TArr));

  // If the access is statically known, don't bother profiling as we'll probably
  // optimize it away completely.
  if (arr->hasConstVal() && key->hasConstVal()) return generic(key);

  static const StaticString s_DictAccess{"DictAccess"};
  static const StaticString s_KeysetAccess{"KeysetAccess"};
  static const StaticString s_MixedArrayAccess{"MixedArrayAccess"};
  auto const profile = TargetProfile<ArrayAccessProfile> {
    env.context,
    env.irb->curMarker(),
    is_dict ? s_DictAccess.get() :
    is_keyset ? s_KeysetAccess.get() :
    s_MixedArrayAccess.get()
  };

  if (profile.profiling()) {
    gen(
      env,
      is_dict ? ProfileDictAccess :
        is_keyset ? ProfileKeysetAccess :
        ProfileMixedArrayAccess,
      ArrayAccessProfileData { profile.handle(), cow_check },
      arr,
      key
    );
    return generic(key);
  }

  if (profile.optimizing()) {
    auto const data = profile.data();

    if (auto const pos = data.choose()) {
      return cond(
        env,
        [&] (Block* taken) {
          SSATmp* marr;
          if (!is_dict && !is_keyset) {
            env.irb->constrainValue(
              arr,
              GuardConstraint(DataTypeSpecialized).setWantArrayKind()
            );
            auto const TMixedArr = Type::Array(ArrayData::kMixedKind);
            marr = gen(env, CheckType, TMixedArr, taken, arr);
          } else {
            marr = arr;
          }

          auto const extra = IndexData { *pos };
          gen(
            env,
            is_dict ? CheckDictOffset :
              is_keyset ? CheckKeysetOffset :
              CheckMixedArrayOffset,
            extra,
            taken,
            marr,
            key
          );

          if (cow_check) {
            gen(env, CheckArrayCOW, taken, marr);
          }
          return marr;
        },
        [&] (SSATmp* tmp) { return direct(tmp, key, *pos); },
        [&] {
          hint(env, Block::Hint::Unlikely);
          return generic(key);
        }
      );
    }
  }
  return generic(key);
}

///////////////////////////////////////////////////////////////////////////////

/*
 * Use TypeProfile to profile the type of `tmp' (typically loaded from
 * the heap) and emit a type check in optimizing translations to
 * refine some properties of the types observed during profiling.
 * Such refinements include checking a specific type in case it's
 * monomorphic, or checking that it's uncounted or unboxed.  In case
 * the check fails dynamically, a side exit is taken.  The `finish'
 * lambda is invoked to emit code before exiting the region at the
 * next bytecode-instruction boundary.
 */
template<class Finish>
SSATmp* profiledType(IRGS& env, SSATmp* tmp, Finish finish) {
  if (tmp->type() <= TGen && tmp->type().isKnownDataType()) {
    return tmp;
  }

  static const StaticString s_TypeProfile{"TypeProfile"};
  TargetProfile<TypeProfile> prof(env.context, env.irb->curMarker(),
                                  s_TypeProfile.get());

  if (prof.profiling()) {
    gen(env, ProfileType, RDSHandleData{ prof.handle() }, tmp);
  }

  if (!prof.optimizing()) return tmp;

  auto const reducedType = prof.data().type;

  if (reducedType == TBottom) {
    // We got no samples
    return tmp;
  }

  Type typeToCheck = relaxToGuardable(reducedType);

  if (typeToCheck == TGen) return tmp;

  SSATmp* ptmp{nullptr};

  ifThen(env,
         [&](Block* taken) {
           ptmp = gen(env, CheckType, typeToCheck, taken, tmp);
         },
         [&] {
           hint(env, Block::Hint::Unlikely);
           auto const takenType = negativeCheckType(tmp->type(), typeToCheck);
           if (takenType < tmp->type()) {
             gen(env, AssertType, takenType, tmp);
           }
           finish();
           gen(env, Jmp, makeExit(env, nextBcOff(env)));
         });

  return ptmp;
}

///////////////////////////////////////////////////////////////////////////////

}}}

#endif
