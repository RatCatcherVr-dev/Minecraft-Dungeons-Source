#include "Dungeons.h"
#include "RollCharge.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

URollCharge::URollCharge() {
	TypeId = EEnchantmentTypeID::RollCharge;

	LevelMultiplier = [this](int level) -> float {
		return Duration + (DurationPerLevel * (level - 1));
	};
	MultiplierFormatter = valueformat::asForRoundedSecond;
}

void URollCharge::OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey key) {
	if (GetOwnerRole() != ROLE_Authority) return;

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);

	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, Effect, 1.0f);
	spec.SetSetByCallerMagnitude(FName("Duration"), LevelMultiplier(Level));
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec, key);
}

URollChargeGameplayEffect::URollChargeGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = FName("Duration");
	DurationMagnitude = durationMagnitude;
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Ranged.AutoCharge"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Ranged.RollCharge"), 0, 1);
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.StatusEffect.Ranged.RollCharge"), 0, 1);
}