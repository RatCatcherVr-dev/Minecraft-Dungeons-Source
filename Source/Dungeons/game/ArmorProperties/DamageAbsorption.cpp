// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "DamageAbsorption.h"
#include "Game/abilities/attributes/HealthAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>

UDamageAbsorptionGameplayEffect::UDamageAbsorptionGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	effects::AddSetByCallerAttribute(*this, UHealthAttributeSet::TakeDamageMultiplierAttribute(), EGameplayModOp::Division);
}


void UDamageAbsorption::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UDamageAbsorptionGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UHealthAttributeSet::TakeDamageMultiplierAttribute().GetName(), 1.f/Absorption);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UDamageAbsorption::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}

UDamageAbsorption::UDamageAbsorption() {
	TypeID = EArmorPropertyID::DamageAbsorption;
}
