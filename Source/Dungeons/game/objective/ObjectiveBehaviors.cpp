#include "Dungeons.h"
#include "ObjectiveBehaviors.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/action/TreeActions.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/MoveBehavior.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"

namespace game { namespace objective { namespace behavior {

using namespace bt;
using namespace bt::behavior;

Unique<UBtNode> moveToClosestPlayerThenPop(const bt::speed::Speed& speed, float popDistance) {
	return moveToThenPop(bt::actor::ClosestPlayer(), speed, popDistance);
}

Unique<UBtNode> moveToThenPop(const bt::locator::Provider& locator, const bt::speed::Speed& speed, float popDistance) {
	auto target = bt::sharedObjectRef<AActor>();

	return parallel("move-to-target-then-pop-behavior",
		every(1s, parallel(
 			set(target, locator),
			sequence(
				isInRange(target, popDistance),
				tree::RemoveThis()
			)
		)),
		moveTo(target, speed)
	);
}

}}}
