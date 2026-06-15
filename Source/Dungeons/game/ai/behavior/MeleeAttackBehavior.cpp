#include "Dungeons.h"
#include "MeleeAttackBehavior.h"
#include "NodeFactory.h"
#include "game/ai/bt/BtLogic.h"
#include "game/ai/action/FocusActions.h"
#include "game/ai/condition/AttackPredicates.h"
#include "game/ai/condition/ActorPredicates.h"
#include "game/ai/condition/BtPredicate.h"
#include "game/ai/provider/Actors.h"
#include "game/ai/task/attack/MeleeAttack.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/MeleeAttackComponent.h"


namespace bt { namespace behavior {

Unique<UBtNode> meleeAttack(AMobCharacter& mob, const actor::Provider& target /* = bt::actor::Target()*/, int index) {
	auto MeleeAttack = mob.FindComponentByClass<UMeleeAttackComponent>();
	return sequence("melee-attack",
		startPredicate(
			attack::InAttackRange(target, MeleeAttack) || attack::IsAttackInProgress(MeleeAttack)
		),
		focus::Set(target, EAIFocusPriority::Attacking),
		onStop({
			focus::Clear(EAIFocusPriority::Attacking)
			}),
		ifElse((actor::IsAlive(target) && attack::InAttackRange(target, MeleeAttack) && attack::InAttackOrientation(target, MeleeAttack)) || attack::IsAttackInProgress(MeleeAttack),
			
			std::make_unique<UMeleeAttack>(target, false, index),
			
			actor::IsAlive(target) && attack::InAttackRange(target, MeleeAttack)
			)
	);
}

}}
