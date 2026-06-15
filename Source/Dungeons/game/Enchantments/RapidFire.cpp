// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "RapidFire.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"

namespace {
	FName RapidFireRangedSpeedEffectMagnitude(TEXT("RapidFireRangedSpeedEffectMagnitude"));
}

URapidFire::URapidFire() {
	TypeId = EEnchantmentTypeID::RapidFire;
	LevelMultiplier = [](int level) -> float {
		return  1.05f + (0.05f * (float)level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

void URapidFire::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(effects::CreateGameplayEffectSpec<URapidFireGameplayEffect>(abilitySystem));
	spec.SetSetByCallerMagnitude(::RapidFireRangedSpeedEffectMagnitude, LevelMultiplier(Level));
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void URapidFire::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}


URapidFireGameplayEffect::URapidFireGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();

	FSetByCallerFloat rangedDamageMagnitude;
	rangedDamageMagnitude.DataName = ::RapidFireRangedSpeedEffectMagnitude;

	rangedDamageInfo.ModifierMagnitude = rangedDamageMagnitude;
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedDamageInfo);
}