#include "Dungeons.h"
#include "BtLogic.h"

namespace bt {

Pred operator!(const Pred& pred) {
	return logicalNot(pred);
}

Pred operator&&(const Pred& a, const Pred& b) {
	return logicalAnd(a, b);
}

Pred operator||(const Pred& a, const Pred& b) {
	return logicalOr(a, b);
}

}
