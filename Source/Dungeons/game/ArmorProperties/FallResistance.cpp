// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "FallResistance.h"
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>

// ----- FALL RESISTANCE GAMEPLAY EFFECT ----- //
UFallResistanceGameplayEffect::UFallResistanceGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Resistance.Fall"));
	effects::AddSetByCallerAttribute(*this, UResistanceAttributeSet::FallResistanceMagnitudeAttribute(), EGameplayModOp::Override);
}

// ----- FALL RESISTANCE ARMOUR PROPERTY ----- //
void UFallResistance::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UFallResistanceGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::FallResistanceMagnitudeAttribute().GetName(), Math::max(0, Resistance));
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UFallResistance::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}

UFallResistance::UFallResistance() {
	TypeID = EArmorPropertyID::FallResistance;
}