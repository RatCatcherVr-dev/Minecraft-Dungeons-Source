#include "Dungeons.h"
#include "VineBehavior.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/RangedAttackBehavior.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Actors.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/util/Tags.h"
#include "game/ai/action/TransformActions.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/HealthActions.h"

#include "game/ai/task/PlayAnimation.h"
#include "game/ai/provider/ActorStats.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/debug/BtDebug.h"
#include "game/ai/action/StateActions.h"

#include "game/ai/provider/BtRandom.h"
#include "game/ai/condition/BtProbability.h"

namespace bt { namespace behavior { namespace entities {

	BehaviorTuple createEntangleVine(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
		return BehaviorTuple(
			sequence(
				bt::state::SetCollisionEnabled(false),
				bt::state::SetIsTargetable(false),
				parallel(
					dropFor(options.Get("lifespan", 4.0s),
						common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); })
					)
				)
			)
		);
	}

}}}
