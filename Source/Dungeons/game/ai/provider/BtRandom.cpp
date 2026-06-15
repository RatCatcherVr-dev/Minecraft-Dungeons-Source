#include "Dungeons.h"
#include "BtRandom.h"
#include "util/Random.h"
#include "util/FloatRange.h"

namespace bt { namespace random {

static Random rnd;

Provider<float> random() {
	return [](StateRef state) { return rnd.nextFloat(); };
}

Provider<float> random(float max) {
	return [max](StateRef state) { return rnd.nextFloat(max); };
}

Provider<float> random(float min, float max) {
	return [min, max](StateRef state) { return rnd.nextFloat(min, max); };
}

Provider<float> random(const FloatRange& range) {
	return [range](StateRef state) { return rnd.nextFloat(range.min(), range.max()); };
}

Provider<int> randomInt(int n) {
	return [n](StateRef state) { return rnd.nextInt(n); };
}

Provider<int> randomInt(int min, int maxExclusive) {
	return [min, maxExclusive](StateRef state) { return rnd.nextInt(min, maxExclusive); };
}

Provider<int> randomIntInclusive(int min, int maxInclusive) {
	return randomInt(min, maxInclusive + 1);
}

Provider<bool> randomBool(float trueWeight) {
	return [trueWeight](StateRef state) { return rnd.nextFloat() < trueWeight; };
}

}}
