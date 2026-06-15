// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "MoveSpeedReduction.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/abilities/attributes/MovementAttributeSet.h"

namespace {
	FName MoveSpeedEffectMagnitude(TEXT("MoveSpeedEffectMagnitude"));
}

void UMoveSpeedReduction::BeginPlay() {
	Super::BeginPlay();

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UMoveSpeedReductionGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext());
	spec.SetSetByCallerMagnitude(::MoveSpeedEffectMagnitude, Multiplier);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UMoveSpeedReduction::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);

	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	auto characterOwner = Cast<ABaseCharacter>(GetOwner());

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UMoveSpeedReduction::UMoveSpeedReduction() {
	TypeID = EArmorPropertyID::MoveSpeedReduction;
	Effect = UMoveSpeedReductionGameplayEffect::StaticClass();
}

UMoveSpeedReductionGameplayEffect::UMoveSpeedReductionGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo speedInfo;
	speedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = ::MoveSpeedEffectMagnitude;

	speedInfo.ModifierMagnitude = speedMagnitude;
	speedInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(speedInfo);
}

