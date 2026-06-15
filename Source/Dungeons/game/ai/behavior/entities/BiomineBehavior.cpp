#include "Dungeons.h"
#include "BiomineBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/bt/decorator/BtEvery.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/condition/IsInRange.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/LocatorPredicates.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/action/HealthActions.h"
#include "game/component/BehaviorOptionsComponent.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

namespace bt { namespace behavior { namespace entities {

BehaviorTuple createBiomine(class AMobCharacter& mob, const UBehaviorOptionsComponent& options) {
	static Random rnd;

	const auto optTriggerRadius = options.Get("TriggerRadius", 200.0f);
	const auto optSecondsTillExplosion = options.Get("SecondsTillExplosion", 2.0s);

	const auto optDuration = rnd.nextFloat( 
		options.Get("MinDuration", 8.0f),
		options.Get("MaxDuration", 10.0f)
	);

	auto triggered = sharedRef<bool>(false);

	return BehaviorTuple(
		root(selector("Biomine",
			uninterruptibleSequence(
				startPredicate( equals( triggered, value(true) ) ),
				parallel(
					behavior::aoeAttack(mob),
					chain(
						delay( 1.0s ),
						health::Kill(actor::Self(), damageTag::explosion(), 1.0f)
					)
				)
			),
			sequence(
				predicate( locator::IsInRange(actor::Target(), optTriggerRadius) ),
				dropFor( optSecondsTillExplosion, set( triggered, value(true) ) )
			),
			sequence(
				predicate( actor::IsRecentlyDamagedFromAttack(10s) ),
				set( triggered, value( true ) )
			)
		)),
		parallel("UpdateTargets",
			every(0.5s,	set(actor::Target(), actor::ClosestEnemy())),
			dropFor( optDuration, set( triggered, value( true ) ) )
		)
); }

}}}
