#include "Dungeons.h"
#include "GeomancerBombBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/action/CommonActions.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createGeomancerBomb(class AMobCharacter& mob) { return BehaviorTuple(
	parallel("GeomancerBomb",
		// phony aoe attack just to play an animation
		// should have a play animation task
		dropFor(2s, behavior::aoeAttack(mob)),
		dropFor(3s, common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); }))
	)
); }

}}}
