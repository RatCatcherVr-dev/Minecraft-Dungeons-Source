// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "FastAttack.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/BaseCharacter.h"

namespace {
	FName FastAttackMeleeSpeedEffectMagnitude(TEXT("FastAttackMeleeSpeedEffectMagnitude"));
	FName FastAttackRangedSpeedEffectMagnitude(TEXT("FastAttackRangedSpeedEffectMagnitude"));
}

UFastAttack::UFastAttack() {
	TypeId = EEnchantmentTypeID::FastAttack;
}

void UFastAttack::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(effects::CreateGameplayEffectSpec<UFastAttackGameplayEffect>(abilitySystem));
	spec.SetSetByCallerMagnitude(::FastAttackMeleeSpeedEffectMagnitude, 2.0f);
	spec.SetSetByCallerMagnitude(::FastAttackRangedSpeedEffectMagnitude, 2.0f);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UFastAttack::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UFastAttackGameplayEffect::UFastAttackGameplayEffect(const FObjectInitializer& ObjectInitializer)
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
	meleeDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute();

	FSetByCallerFloat meleeDamageMagnitude;
	meleeDamageMagnitude.DataName = ::FastAttackMeleeSpeedEffectMagnitude;

	meleeDamageInfo.ModifierMagnitude = meleeDamageMagnitude;
	meleeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeDamageInfo);

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();

	FSetByCallerFloat rangedDamageMagnitude;
	rangedDamageMagnitude.DataName = ::FastAttackRangedSpeedEffectMagnitude;

	rangedDamageInfo.ModifierMagnitude = rangedDamageMagnitude;
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedDamageInfo);
}