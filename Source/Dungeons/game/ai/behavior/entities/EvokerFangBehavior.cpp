#include "Dungeons.h"
#include "EvokerFangBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/action/CommonActions.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createEvokerFang(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) { return BehaviorTuple(
	parallel("EvokerFang",
		dropFor(options.Get("AoeDelay", 1.7s), behavior::aoeAttack(mob)),
		dropFor(options.Get("DieDelay", 3.9s), common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); }))
	)
); }

}}}
