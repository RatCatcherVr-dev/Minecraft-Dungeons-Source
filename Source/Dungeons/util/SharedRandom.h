#pragma once

#include "Random.h"

namespace Util {

Random& sharedRandom();
Random* thisOrSharedRandom(Random*);

}
