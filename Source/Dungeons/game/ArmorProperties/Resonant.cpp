#include "Resonant.h"
#include "game/abilities/attributes/StatusAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>

namespace resonant {
	FName BuffExtensionMagnitude(TEXT("BuffExtensionMagnitude"));
}

// ----- Gameplay Effect ----- //

UResonantGameplayEffect::UResonantGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo buffExtensionInfo;
	buffExtensionInfo.Attribute = UStatusAttributeSet::PositiveStatusDurationMagnitudeAttribute();

	FSetByCallerFloat extensionMagnitude;
	extensionMagnitude.DataName = resonant::BuffExtensionMagnitude;

	buffExtensionInfo.ModifierMagnitude = extensionMagnitude;
	buffExtensionInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(buffExtensionInfo);
}

// ----- Armour Property ----- //

UResonant::UResonant() {
	TypeID = EArmorPropertyID::Resonant;
}

void UResonant::BeginPlay() {
	Super::BeginPlay();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	if (auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner())) {

		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UResonantGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(resonant::BuffExtensionMagnitude, mResistMagnitude);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UResonant::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	if (GetOwnerRole() != ROLE_Authority)
		return;

	if (auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner())) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}
