// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "CaveSpiderPoisonEnchantment.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/component/HealthComponent.h"
#include <GameplayEffect.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "util/CharacterQuery.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"

namespace {
	FName HealthDrainEffectMagnitude(TEXT("HealthDrainEffectMagnitude"));
	FName DurationEffectMagnitude(TEXT("DurationEffectMagnitude"));
}

UCaveSpiderPoisonEnchantment::UCaveSpiderPoisonEnchantment() {
	Effect = UCaveSpiderPoisonedGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::CaveSpiderPoisonEnchantment;
	PredictiveExecution = true;
}

void UCaveSpiderPoisonEnchantment::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (missedAttack) {
		return;
	}
	const auto target = Cast<ABaseCharacter>(toWhat);
	auto ownerCharacter = GetCharacterOwner();

	if (target && actorquery::is::alive(target) && characterquery::is::targetable(target)) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();

		auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();
		FGameplayEffectSpec spec(Cast<UCaveSpiderPoisonedGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
		spec.SetSetByCallerMagnitude(effects::DurationName, 10.0f);
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent(), context.GetKey());
	}
}

UCaveSpiderPoisonedGameplayEffect::UCaveSpiderPoisonedGameplayEffect(const FObjectInitializer& ObjectInitializer)
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

	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.5f;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UDifficultyDamageModCalculation::StaticClass();
	healthMagnitude.Coefficient = FScalableFloat(-damagePerSecond * Period.GetValueAtLevel(1));

	FGameplayModifierInfo healthAdditionInfo;
	healthAdditionInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthAdditionInfo.ModifierMagnitude = healthMagnitude;
	healthAdditionInfo.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(healthAdditionInfo);

	const auto weakDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak"));
	InheritableGameplayEffectTags.AddTag(weakDamageTag);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	damage.PassedInTags.AddTag(weakDamageTag);
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Poison"), 0, 1);
}