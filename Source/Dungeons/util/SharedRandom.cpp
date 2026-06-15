#include "Dungeons.h"
#include "SharedRandom.h"

namespace Util {

Random& sharedRandom() {
	static Random random;
	return random;
}

Random* thisOrSharedRandom(Random* rnd) {
	return rnd ? rnd : &sharedRandom();
}

}
