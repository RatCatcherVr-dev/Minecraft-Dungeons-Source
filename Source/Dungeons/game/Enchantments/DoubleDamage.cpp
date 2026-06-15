// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DoubleDamage.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

namespace {
	FName DoubleDamageMeleeDamageEffectMagnitude(TEXT("DoubleDamageMeleeDamageEffectMagnitude"));
	FName DoubleDamageRangedDamageEffectMagnitude(TEXT("DoubleDamageRangedDamageEffectMagnitude"));
}

UDoubleDamage::UDoubleDamage() {
	TypeId = EEnchantmentTypeID::DoubleDamage;
}

void UDoubleDamage::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(effects::CreateGameplayEffectSpec<UDoubleDamageGameplayEffect>(abilitySystem));
	spec.SetSetByCallerMagnitude(::DoubleDamageMeleeDamageEffectMagnitude, 2.0f);
	spec.SetSetByCallerMagnitude(::DoubleDamageRangedDamageEffectMagnitude, 2.0f);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UDoubleDamage::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UDoubleDamageGameplayEffect::UDoubleDamageGameplayEffect(const FObjectInitializer& ObjectInitializer)
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
	meleeDamageMagnitude.DataName = ::DoubleDamageMeleeDamageEffectMagnitude;

	meleeDamageInfo.ModifierMagnitude = meleeDamageMagnitude;
	meleeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(meleeDamageInfo);

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();

	FSetByCallerFloat rangedDamageMagnitude;
	rangedDamageMagnitude.DataName = ::DoubleDamageMeleeDamageEffectMagnitude;

	rangedDamageInfo.ModifierMagnitude = rangedDamageMagnitude;
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(rangedDamageInfo);
}
