// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "RangedDamageBoost.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/RangedAttributeSet.h"

namespace {
	FName RangedDamageEffectMagnitude(TEXT("RangedDamageEffectMagnitude"));
}

void URangedDamageBoost::BeginPlay() {
	Super::BeginPlay();

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<URangedDamageBoostGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext());
	spec.SetSetByCallerMagnitude(::RangedDamageEffectMagnitude, Multiplier);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void URangedDamageBoost::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

URangedDamageBoost::URangedDamageBoost() {
	TypeID = EArmorPropertyID::RangedDamageBoost;
	Effect = URangedDamageBoostGameplayEffect::StaticClass();
}

URangedDamageBoostGameplayEffect::URangedDamageBoostGameplayEffect(const FObjectInitializer& ObjectInitializer)
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
	damageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat damageMagnitude;
	damageMagnitude.DataName = ::RangedDamageEffectMagnitude;

	damageInfo.ModifierMagnitude = damageMagnitude;
	damageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(damageInfo);
}



