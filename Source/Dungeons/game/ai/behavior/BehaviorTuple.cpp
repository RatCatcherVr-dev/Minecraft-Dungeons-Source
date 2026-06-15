#include "Dungeons.h"
#include "BehaviorTuple.h"

namespace bt {

void BehaviorTuple::moveTo(UBehaviorComponent* behaviorComponent) {
	if (targets) {
		behaviorComponent->Targets().Push(std::move(targets));
	}
	if (actions) {
		behaviorComponent->Actions().Push(std::move(actions));
	}
}

}
