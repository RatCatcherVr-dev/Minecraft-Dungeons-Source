// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Swiftfooted.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "../abilities/effects/calculations/StatusModCalculations.h"

namespace {
	FName SwiftfootedEffectDuration(TEXT("SwiftfootedEffectDuration"));
	FName SwiftfootedEffectMagnitude(TEXT("SwiftfootedEffectMagnitude"));
}

USwiftfooted::USwiftfooted() {
	TypeId = EEnchantmentTypeID::Swiftfooted;

	LevelMultiplier = [this](int level) -> float {
		return 1.2f + (0.1f * (float)level);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

FText USwiftfooted::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(swiftDuration)));
}

void USwiftfooted::OnDodgeRollEnd(FPredictionKey key) {
	ApplyEffect(key);

	if (GetOwner()->HasAuthority()) {
		BroadcastEnchantmentTriggeredEvent();
	}
}



void USwiftfooted::ApplyEffect(FPredictionKey key) {
	if (const auto* characterOwner = GetCharacterOwner()) {
		if (auto abilitySystem = characterOwner->GetAbilitySystemComponent()) {
			FGameplayEffectSpec spec(effects::CreateGameplayEffectSpec<USwiftfootedGameplayEffect>(abilitySystem, Level));
			spec.SetSetByCallerMagnitude(effects::DurationName, swiftDuration);
			spec.SetSetByCallerMagnitude(::SwiftfootedEffectMagnitude, LevelMultiplier(Level));
			abilitySystem->ApplyGameplayEffectSpecToSelf(spec, key);
		}
	}
}

USwiftfootedGameplayEffect::USwiftfootedGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = ::SwiftfootedEffectMagnitude;

	info.ModifierMagnitude = speedMagnitude;
	info.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(info);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Swiftfooted"), 0, 1);
}