#include "Dungeons.h"
#include "Enchant.h"
#include "game/actor/character/BaseCharacterStates.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/ai/provider/Actors.h"
#include "game/component/GrowAttackComponent.h"
#include "game/actor/character/mob/MobCharacter.h"

UEnchant::UEnchant(const bt::actor::Provider& target)
	: targetProvider(bt::actor::toMulti(target)) {
	type = "enchant";
}

UEnchant::UEnchant(const bt::actor::MultiProvider& target)
	: targetProvider(target) {
	type = "enchant";
}

void UEnchant::Init(bt::StateRef state) {
	enchantComponent = state.owner->FindComponentByClass<UGrowAttackComponent>();
}

bool UEnchant::OnCanRun(bt::StateRef state) {
	return enchantComponent != nullptr && enchantComponent->CanGrow(targetProvider(state));
}

bool UEnchant::OnCanContinue(bt::StateRef state) {
	return !(isDone && currentAttackCounter.update(enchantComponent->SuccessfulAttackCounter));
}

void UEnchant::OnStart(bt::StateRef state) {
	isDone = false;
}

void UEnchant::OnTick(bt::StateRef state) {
	auto targets = targetProvider(state);

	if (enchantComponent->CanGrow(targets)) {
		currentAttackCounter = enchantComponent->SuccessfulAttackCounter;
		enchantComponent->Grow(targets);
		state.owner->SetAttackState(EAttackState::Melee);
		isDone = true;
	}
}

void UEnchant::OnStop(bt::StateRef state) {
	state.owner->SetAttackState(EAttackState::None);
}
