// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "Protecting.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

namespace {
	FName ProtectingTakeDamageEffectMagnitude(TEXT("ProtectingTakeDamageMagnitude"));
}

UProtecting::UProtecting() {
	Effect = UProtectingGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Protection;

	LevelMultiplier = [this](int level) -> float {
		return 1.0f + (0.06f * level);
	};
	MultiplierFormatter = valueformat::asRelativeDividerPercentageChange;
}

void UProtecting::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	FGameplayEffectSpec spec(Cast<UProtectingGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
	spec.SetSetByCallerMagnitude(::ProtectingTakeDamageEffectMagnitude, GetOwner()->IsA(APlayerCharacter::StaticClass()) ? LevelMultiplier(Level) : 1.75f);
	Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
}

void UProtecting::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;

	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
	abilitySystem->RemoveActiveGameplayEffect(Handle);
}

UProtectingGameplayEffect::UProtectingGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo takeDamageInfo;
	takeDamageInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();

	FSetByCallerFloat takeDamageMagnitude;
	takeDamageMagnitude.DataName = ::ProtectingTakeDamageEffectMagnitude;

	takeDamageInfo.ModifierMagnitude = takeDamageMagnitude;
	takeDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(takeDamageInfo);
}