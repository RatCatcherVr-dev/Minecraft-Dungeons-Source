// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "SlowResistance.h"
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>

// ----- SLOW RESISTANCE GAMEPLAY EFFECT ----- //
USlowResistanceGameplayEffect::USlowResistanceGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Resistance.Slow"));
	effects::AddSetByCallerAttribute(*this, UResistanceAttributeSet::SlowResistanceMagnitudeAttribute(), EGameplayModOp::Multiplicitive);
}

// ----- SLOW RESISTANCE ARMOUR PROPERTY ----- //
void USlowResistance::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USlowResistanceGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::SlowResistanceMagnitudeAttribute().GetName(), Math::max(0, 1 - Resistance));
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void USlowResistance::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}

USlowResistance::USlowResistance() {
	TypeID = EArmorPropertyID::SlowResistance;
}

// ----- SLOW RESISTANCE ENCHANTMENT ----- //
UEnchantmentSlowResistance::UEnchantmentSlowResistance() {
	TypeId = EEnchantmentTypeID::SlowResistance;

	LevelMultiplier = [this](int level) -> float {
		return Math::min(1, Resistance + PerLevelResistance * (1 - level));
	};
	MultiplierFormatter = valueformat::asMultiplierPercentageChange;
}

FText UEnchantmentSlowResistance::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asEveryRoundedWordSecond(LevelMultiplier(Level))));
}

void UEnchantmentSlowResistance::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<USlowResistanceGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::SlowResistanceMagnitudeAttribute().GetName(), 1 - LevelMultiplier(Level));
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UEnchantmentSlowResistance::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}
