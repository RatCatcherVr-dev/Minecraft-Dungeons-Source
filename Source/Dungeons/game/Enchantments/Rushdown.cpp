// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Rushdown.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

namespace {
	FName RushdownEffectDuration(TEXT("SwiftfootedEffectDuration"));
	FName RushdownEffectMagnitude(TEXT("SwiftfootedEffectMagnitude"));
}

URushdown::URushdown() {
	TypeId = EEnchantmentTypeID::Rushdown;
	MultiplierFormatter = valueformat::asDurationRoundedSecond;
	Effect = URushdownGameplayEffect::StaticClass();
}

void URushdown::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext window) {
	auto owner = GetOwner();
	if (owner->Role != ROLE_Authority)
		return;
	if (auto whomCharacter = Cast<ABaseCharacter>(toWhom)) {
		if (!actorquery::is::alive(whomCharacter)) {
			BoostSpeed();
		}
	}
}

void URushdown::BoostSpeed() {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, Effect);
		spec.SetSetByCallerMagnitude(::RushdownEffectMagnitude, speedBonus);
		spec.SetSetByCallerMagnitude(effects::DurationName, speedDurationAfterKill);
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);

		if (GetOwner()->HasAuthority()) {
			BroadcastEnchantmentTriggeredEvent();
		}
	}
}

URushdownGameplayEffect::URushdownGameplayEffect(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = ::RushdownEffectMagnitude;

	info.ModifierMagnitude = speedMagnitude;
	info.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(info);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Rushdown"), 0, 1);
}