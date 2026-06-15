// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "MeleeDamageBoost.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MeleeAttributeSet.h"

namespace {
	FName MeleeDamageEffectMagnitude(TEXT("MeleeDamageEffectMagnitude"));
}

void UMeleeDamageBoost::BeginPlay() {
	Super::BeginPlay();

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UMeleeDamageBoostGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext());
	spec.SetSetByCallerMagnitude(::MeleeDamageEffectMagnitude, Multiplier);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UMeleeDamageBoost::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UMeleeDamageBoost::UMeleeDamageBoost() {
	TypeID = EArmorPropertyID::MeleeDamageBoost;
	Effect = UMeleeDamageBoostGameplayEffect::StaticClass();
}

UMeleeDamageBoostGameplayEffect::UMeleeDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo damageInfo;
	damageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat damageMagnitude;
	damageMagnitude.DataName = ::MeleeDamageEffectMagnitude;

	damageInfo.ModifierMagnitude = damageMagnitude;
	damageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(damageInfo);
}

