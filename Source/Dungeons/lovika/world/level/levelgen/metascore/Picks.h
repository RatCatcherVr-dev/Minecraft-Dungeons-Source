#pragma once

#include "MetaScoreTypes.h"
#include <Optional.h>
#include <limits>

namespace levelgen { namespace score { namespace pick {

const Picker& First();
      Picker  FirstAtLeast(float minimum, bool orBest = true);
      Picker  Best(int successfulCount = std::numeric_limits<int>::max());
      Picker  Random(int successfulCount);

      Picker  Max(int successfulCount = std::numeric_limits<int>::max());
      Picker  Min(int successfulCount = std::numeric_limits<int>::max());

}}}
