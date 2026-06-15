#include "Dungeons.h"
#include "AnimalBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/FleeBehavior.h"
#include "game/ai/behavior/RoamBehavior.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/provider/Locations.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createDefaultAnimal(AMobCharacter&) { return BehaviorTuple(
	selector("flee-or-roam",
		sequence("flee-condition",
			startPredicate(actor::IsRecentlyDamagedFromAttack(10s)),
			behavior::defaultFleeFrom(actor::LastAttacker())
		),
		behavior::defaultRoam()
	),
	every(5s,
		set(location::Anchor(), location::Self())
	));
}

}}}
