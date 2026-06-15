#include "Dungeons.h"
#include "BtEvalState.h"
#include "game/GameBP.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/mob/MobBtController.h"
#include "game/actor/character/CharacterAnimInstance.h"
#include "game/component/MobAnimationsComponent.h"
#include "game/util/ActorQuery.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UWorld& FBtEvalState::world() const {
	return *owner->GetWorld();
}

AGameBP& FBtEvalState::game() const {
	return *actorquery::getFirstActor<AGameBP>(owner->GetWorld());
}

const game::FDifficulty& FBtEvalState::difficulty() const {
	return game().GetGame()->settings().difficulty;
}

FMobParams& FBtEvalState::params() const {
	return owner->MobParams;
}

class UCharacterAnimInstance* FBtEvalState::anim() const { // @btlifecycle
	if (!owner->AnimInstance.IsValid()) {
		return nullptr;
	}

	return owner->AnimInstance.Get();
}

UMobAnimationsComponent* FBtEvalState::animPack() const { // @btlifecycle
	if (!owner->AnimInstance.IsValid()) {
		return nullptr;
	}
	return owner->FindComponentByClass<UMobAnimationsComponent>();
}

FBtEvalState bt::createState(AMobCharacter& mob, int tickId) {
	return FBtEvalState (
		tickId,
		&mob,
		static_cast<AMobBtController*>(mob.AiController()),
		static_cast<UCharacterMovementComponent*>(mob.GetMovementComponent()),
		nullptr
	);
}

FBtEvalState bt::internal::copyStateWithTickId(StateRef state, int tickId) {
	return FBtEvalState (tickId,state.owner,state.controller,state.movement, nullptr);
}

FBtEvalState bt::internal::copyStateWithCurrentTickId(StateRef state) {
	return copyStateWithTickId(state, currentTickId);
}

int bt::internal::currentTickId = 0;
