#pragma once

#include "TrialTypes.h"

typedef uint32_t RandomSeed;


namespace trial { namespace mock {

TArray<FTrialDef> generateRandom(RandomSeed, TOptional<int> count);

}}
