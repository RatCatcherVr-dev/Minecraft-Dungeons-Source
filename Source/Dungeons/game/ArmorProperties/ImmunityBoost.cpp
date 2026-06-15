#include "ImmunityBoost.h"
#include "game/abilities/attributes/StatusAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>

namespace immunityboost {
	FName BuffExtensionMagnitude(TEXT("BuffExtensionMagnitude"));
}

// ----- Gameplay Effect ----- //

UImmunityBoostGameplayEffect::UImmunityBoostGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo buffExtensionInfo;
	buffExtensionInfo.Attribute = UStatusAttributeSet::NegativeStatusDurationMagnitudeAttribute();

	FSetByCallerFloat extensionMagnitude;
	extensionMagnitude.DataName = immunityboost::BuffExtensionMagnitude;

	buffExtensionInfo.ModifierMagnitude = extensionMagnitude;
	buffExtensionInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(buffExtensionInfo);
}

// ----- Enchantment ----- //

UImmunityBoost::UImmunityBoost() {
	TypeID = EArmorPropertyID::Resonant;
}

void UImmunityBoost::BeginPlay() {
	Super::BeginPlay();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	if (auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner())) {
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UImmunityBoostGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(immunityboost::BuffExtensionMagnitude, mResistanceMagnitude);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UImmunityBoost::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	if (GetOwnerRole() != ROLE_Authority)
		return;

	if (auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner())) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}
