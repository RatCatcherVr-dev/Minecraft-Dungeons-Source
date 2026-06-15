// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Quick.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "../abilities/attributes/MovementAttributeSet.h"
#include "../abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include "game/actor/character/BaseCharacter.h"

namespace {
	FName QuickMoveSpeedEffectMagnitude(TEXT("QuickMoveSpeedEffectMagnitude"));
}

UQuick::UQuick() {
	TypeId = EEnchantmentTypeID::Quick;
}

void UQuick::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(effects::CreateGameplayEffectSpec<UQuickGameplayEffect>(abilitySystem));
	spec.SetSetByCallerMagnitude(::QuickMoveSpeedEffectMagnitude, 2.0f);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UQuick::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UQuickGameplayEffect::UQuickGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo moveSpeedInfo;
	moveSpeedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat moveSpeedMagnitude;
	moveSpeedMagnitude.DataName = ::QuickMoveSpeedEffectMagnitude;

	moveSpeedInfo.ModifierMagnitude = moveSpeedMagnitude;
	moveSpeedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(moveSpeedInfo);
}