#include "Dungeons.h"
#include "BtProbability.h"
#include "util/Random.h"

namespace bt { namespace random {

Pred probability(float prob) {
    static Random rnd;
    return [prob](StateRef) {
        return rnd.nextFloat() < prob;
	};
}

Pred probability(const Provider<float>& provider) {
    static Random rnd;
    return [provider](StateRef state) {
        return rnd.nextFloat() < provider(state);
	};
}

}}
