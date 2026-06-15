// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "MeleeAttackSpeedBoost.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MeleeAttributeSet.h"

namespace {
	FName MeleeAttackSpeedEffectMagnitude(TEXT("MeleeAttackSpeedEffectMagnitude"));
}

void UMeleeAttackSpeedBoost::BeginPlay() {
	Super::BeginPlay();

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UMeleeAttackSpeedBoostGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext());
	spec.SetSetByCallerMagnitude(::MeleeAttackSpeedEffectMagnitude, Multiplier);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UMeleeAttackSpeedBoost::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UMeleeAttackSpeedBoost::UMeleeAttackSpeedBoost() {
	TypeID = EArmorPropertyID::MeleeAttackSpeedBoost;
	Effect = UMeleeAttackSpeedBoostGameplayEffect::StaticClass();
}

UMeleeAttackSpeedBoostGameplayEffect::UMeleeAttackSpeedBoostGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo attackSpeedInfo;
	attackSpeedInfo.Attribute = UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute();

	FSetByCallerFloat attackSpeedMagnitude;
	attackSpeedMagnitude.DataName = ::MeleeAttackSpeedEffectMagnitude;

	attackSpeedInfo.ModifierMagnitude = attackSpeedMagnitude;
	attackSpeedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(attackSpeedInfo);
}