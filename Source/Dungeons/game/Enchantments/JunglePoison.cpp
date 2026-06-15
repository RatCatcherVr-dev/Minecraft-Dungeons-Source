// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "JunglePoison.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/component/HealthComponent.h"
#include <GameplayEffect.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/abilities/effects/executions/DamageExecutionCalculation.h"
#include "util/CharacterQuery.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

namespace {
	FName MagDamagePerSecond(TEXT("Health"));
	FName MagDuration(TEXT("MagDuration"));
}

UJunglePoisonGameplayEffect::UJunglePoisonGameplayEffect(const FObjectInitializer& ObjectInitializer)
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
	healthMagnitude.CalculationClassMagnitude = UMeleeModDamageCalculation::StaticClass();
	FGameplayModifierInfo healthAdditionInfo;
	healthAdditionInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthAdditionInfo.ModifierMagnitude = healthMagnitude;
	healthAdditionInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthAdditionInfo);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Poison")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium.Poison")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.JunglePoison"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Damage.Medium"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UJunglePoisonMelee::UJunglePoisonMelee() {
	Effect = UJunglePoisonGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::JunglePoisonMelee;
	PredictiveExecution = true;
}

void UJunglePoisonMelee::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	if (missedAttack) {
		return;
	}
	const auto target = Cast<ABaseCharacter>(toWhat);
	auto ownerCharacter = GetCharacterOwner();

	if (target && actorquery::is::alive(target) && characterquery::is::targetable(target)) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
		auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();

		FGameplayEffectSpec spec(Cast<UJunglePoisonGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
		spec.SetSetByCallerMagnitude( ::MagDamagePerSecond, -Damage * DamagePercentage * (Level > 0 ? LevelMultiplier(Level) : 1));
		spec.SetSetByCallerMagnitude( effects::DurationName, Duration );
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent(), context.GetKey());
	}
}

UJunglePoisonRanged::UJunglePoisonRanged() {
	Effect = UJunglePoisonGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::JunglePoisonRanged;
	PredictiveExecution = true;
}

void UJunglePoisonRanged::OnAfterDealtRangedDamage( float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream ) {
	const auto target = Cast<ABaseCharacter>(toWhat);
	auto ownerCharacter = GetCharacterOwner();

	if (target && actorquery::is::alive(target) && characterquery::is::targetable(target)) {
		if (GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		auto abilitySystem = ownerCharacter->GetAbilitySystemComponent();

		FGameplayEffectSpec spec(Cast<UJunglePoisonGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
		spec.SetSetByCallerMagnitude( ::MagDamagePerSecond, -Damage * DamagePercentage);
		spec.SetSetByCallerMagnitude( effects::DurationName, Duration );
		abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent());
	}
}

