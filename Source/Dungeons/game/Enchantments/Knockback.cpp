#include "Dungeons.h"
#include "Knockback.h"
#include "game/util/ValueFormat.h"
#include "game/actor/character/player/PlayerCharacter.h"

UKnockback::UKnockback() {
	TypeId = EEnchantmentTypeID::Knockback;
	LevelMultiplier = [this](int level) -> float {
		return 1.0f / (10.0f / (float)level);
	};
	MultiplierFormatter = valueformat::asPercentageChance;
}

void UKnockback::OnBeforeDealtMeleeDamage(float &outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	if ((randStream.FRandRange(0.0f, 1.0f) <= LevelMultiplier(Level)) || bAlwaysTrigger) {
		outPushbackMultiplier *= knockbackPower;
		BroadcastEnchantmentTriggeredEvent();
	}
}