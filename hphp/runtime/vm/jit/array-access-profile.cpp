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

#include "hphp/runtime/vm/jit/array-access-profile.h"

#include "hphp/runtime/base/array-data.h"
#include "hphp/runtime/base/mixed-array.h"
#include "hphp/runtime/base/mixed-array-defs.h"
#include "hphp/runtime/base/set-array.h"
#include "hphp/runtime/base/runtime-option.h"
#include "hphp/runtime/base/string-data.h"
#include "hphp/runtime/vm/member-operations.h"

#include "hphp/util/safe-cast.h"

#include <folly/Optional.h>

#include <algorithm>
#include <cstring>
#include <sstream>

namespace HPHP { namespace jit {

///////////////////////////////////////////////////////////////////////////////

std::string ArrayAccessProfile::toString() const {
  if (!m_init) return std::string("uninitialized");
  std::ostringstream out;
  for (auto const& line : m_hits) {
    out << folly::format("{}:{},", line.pos, line.count);
  }
  out << folly::format("untracked:{}", m_untracked);
  return out.str();
}

folly::Optional<uint32_t>
ArrayAccessProfile::choose() const {
  Line hottest;
  uint32_t total = 0;

  if (!m_init) return folly::none;

  for (auto i = 0; i < kNumTrackedSamples; ++i) {
    auto const& line = m_hits[i];
    if (!validPos(line.pos)) break;

    if (line.count > hottest.count) hottest = line;
    total += line.count;
  }
  total += m_untracked;

  auto const bound = total * RuntimeOption::EvalHHIRMixedArrayProfileThreshold;
  return hottest.count >= bound
    ? folly::make_optional(safe_cast<uint32_t>(hottest.pos))
    : folly::none;
}

bool ArrayAccessProfile::update(int32_t pos, uint32_t count) {
  if (!m_init) init();

  if (!validPos(pos)) {
    m_untracked += count;
    return false;
  }

  for (auto i = 0; i < kNumTrackedSamples; ++i) {
    auto& line = m_hits[i];

    if (line.pos == pos) {
      line.count += count;
      return true;
    }
    if (line.pos == -1) {
      line.pos = pos;
      line.count = count;
      return true;
    }
  }

  m_untracked += count;
  return false;
}

void ArrayAccessProfile::update(const ArrayData* ad, int64_t i, bool cowCheck) {
  // We shouldn't count accesses that would fail to be detected by the offset
  // optimization.
  if (cowCheck && ad->cowCheck()) {
    update(-1, 1);
    return;
  }
  auto h = hash_int64(i);
  auto const pos =
    ad->hasMixedLayout() ? MixedArray::asMixed(ad)->find(i, h) :
    ad->isKeyset() ? SetArray::asSet(ad)->find(i, h) :
    -1;
  update(pos, 1);
}

void ArrayAccessProfile::update(const ArrayData* ad, const StringData* sd,
                                bool cowCheck) {
  // We shouldn't count accesses that would fail to be detected by the offset
  // optimization.  These include cases that require a COW, and cases where we
  // need to walk the string.
  if (cowCheck && ad->cowCheck()) {
    update(-1, 1);
    return;
  }
  auto pos = -1;
  if (ad->hasMixedLayout()) {
    pos = MixedArray::asMixed(ad)->find(sd, sd->hash());
    if (pos != -1 && MixedArray::asMixed(ad)->data()[pos].strKey() != sd) {
      pos = -1;
    }
  } else if (ad->isKeyset()) {
    pos = SetArray::asSet(ad)->find(sd, sd->hash());
    if (pos != -1 && SetArray::asSet(ad)->data()[pos].strKey() != sd) {
      pos = -1;
    }
  }
  update(pos, 1);
}

void ArrayAccessProfile::reduce(ArrayAccessProfile& l,
                                const ArrayAccessProfile& r) {
  if (!r.m_init) return;

  Line scratch[2 * kNumTrackedSamples];
  auto n = uint32_t{0};

  for (auto i = 0; i < kNumTrackedSamples; ++i) {
    auto const& rline = r.m_hits[i];
    if (!validPos(rline.pos)) break;

    // Update `l'.  If `l' can't record the update, save it to scratch.
    if (!l.update(rline.pos, rline.count)) {
      scratch[n++] = rline;
    }
  }
  l.m_untracked += r.m_untracked;

  if (n == 0) return;
  assertx(n <= kNumTrackedSamples);

  // Sort the combined samples, then copy the top hits back into `l'.
  std::memcpy(&scratch[n], &l.m_hits[0],
              kNumTrackedSamples * sizeof(Line));
  std::sort(&scratch[0], &scratch[n + kNumTrackedSamples],
            [] (Line a, Line b) { return a.count > b.count; });
  std::memcpy(l.m_hits, scratch, kNumTrackedSamples);

  // Add the hits in the discarded tail to m_untracked.
  for (auto i = kNumTrackedSamples; i < n + kNumTrackedSamples; ++i) {
    auto const& line = scratch[i];
    if (!validPos(line.pos)) break;

    l.m_untracked += line.count;
  }
}

void ArrayAccessProfile::init() {
  assertx(!m_init);
  for (auto i = 0; i < kNumTrackedSamples; ++i) {
    m_hits[i] = Line{};
  }
  m_init = true;
}

///////////////////////////////////////////////////////////////////////////////

}}
