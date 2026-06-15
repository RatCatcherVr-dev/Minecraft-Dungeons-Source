// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "SummonedMobGameplayEffect.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"

USummonedMobGameplayEffect::USummonedMobGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo meleeDamageInfo;
	meleeDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat meleeDamageMagnitude;
	meleeDamageMagnitude.DataName = "SummonEffectMeleeDamage";

	meleeDamageInfo.ModifierMagnitude = meleeDamageMagnitude;
	meleeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeDamageInfo);

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat rangedDamageMagnitude;
	rangedDamageMagnitude.DataName = "SummonEffectRangedDamage";

	rangedDamageInfo.ModifierMagnitude = rangedDamageMagnitude;
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedDamageInfo);
}
