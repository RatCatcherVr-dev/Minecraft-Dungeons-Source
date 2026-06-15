#include "Dungeons.h"
#include "NodeFactory.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/provider/Locators.h"
#include "game/ai/task/attack/BeamAttack.h"
#include "game/component/BeamAttackComponent.h"
#include "../action/FocusActions.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "../bt/group/BtGroup.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/condition/IsInRange.h"

namespace bt {
	namespace behavior {

		Unique<UBtGroup> beamAttack(AMobCharacter& mob) {
			return sequence("beam-attack",
				std::make_unique<UBeamAttack>()
			);
		}

		Unique<UBtGroup> beamAttackInOrientation(const ABaseCharacter& character, const actor::Provider& target /*= actor::Target()*/, int maxAttacksToIssue /*= 1*/)
		{
			auto attackComponent = character.FindComponentByClass<UBeamAttackComponent>();
			if (!attackComponent) {
				return {};
			}
			return sequence("beam-attack",
				focus::Set(target, EAIFocusPriority::Attacking),
				onStop({focus::Clear(EAIFocusPriority::Attacking)}),
				ifElse((actor::IsAlive(target) && attack::InAttackOrientation(target, attackComponent)) || attack::IsAttackInProgress(attackComponent),
					make_unique<UBeamAttack>(target, maxAttacksToIssue),
					actor::IsAlive(target)
				)
			);
		}

	}
}