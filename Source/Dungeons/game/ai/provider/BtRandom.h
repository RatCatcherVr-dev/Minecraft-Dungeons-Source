#pragma once

#include "game/ai/bt/BtTypes.h"

class FloatRange;

namespace bt { namespace random {

Provider<float> random();
Provider<float> random(float max);
Provider<float> random(float min, float max);
Provider<float> random(const FloatRange&);

Provider<int> randomInt(int n);
Provider<int> randomInt(int min, int maxExclusive);
Provider<int> randomIntInclusive(int min, int maxInclusive);

Provider<bool> randomBool(float trueWeight = .5f);

}}
