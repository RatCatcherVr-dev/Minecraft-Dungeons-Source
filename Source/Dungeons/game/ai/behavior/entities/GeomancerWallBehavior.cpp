#include "Dungeons.h"
#include "GeomancerWallBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/action/CommonActions.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createGeomancerWall(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
	parallel("GeomancerWall",
		// phony aoe attack just to play an animation
		// should have a play animation task
		dropFor(options.Get("Lifetime", 10.s) - 1.142s, behavior::aoeAttack(mob)),
		dropFor(options.Get("Lifetime", 10.s), common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); }))
	)
); }

}}}
