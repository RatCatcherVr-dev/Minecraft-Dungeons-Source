#include "Dungeons.h"
#include "game/util/Tags.h"
#include "AbominationVineBehavior.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/bt/decorator/BtDropFor.h"
#include "game/ai/bt/BtDelay.h"
#include "game/ai/behavior/NodeFactory.h"
#include "game/ai/behavior/AoeAttackBehavior.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/action/CommonActions.h"
#include "game/ai/task/PlayAnimation.h"
#include "game/component/BehaviorOptionsComponent.h"

class UAnimationSequenceBase;

namespace bt { namespace behavior { namespace entities {
	BehaviorTuple createAbominationVine(AMobCharacter& mob, const UBehaviorOptionsComponent& options) {

		const auto optAOEDelay = options.Get( "aoe-delay", 1.0s );
		const auto optDestroyDelay = options.Get( "destroy-delay", 3.0s );
			
		return BehaviorTuple( sequence(
			chain(
				delay( optAOEDelay ),
				behavior::aoeAttack( mob ),
				delay( 1000.0s )
			),
			chain(
				delay( optDestroyDelay ),
				common::Apply(actor::Self(), [](AActor* mob) { mob->Destroy(); })
			)
		) );
	}
} } }
