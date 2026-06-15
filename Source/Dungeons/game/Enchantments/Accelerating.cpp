// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "Accelerating.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

static float AcceleratingLevelMultiplier(int level) {
	return 0.06 + (0.02f * level);
}

static constexpr float MobSpeedIncrease = 0.33f;
static constexpr float SpeedLimit = 2.0f;

namespace {
	FName AcceleratingRangedSpeedEffectMagnitude(TEXT("AcceleratingRangedSpeedEffectMagnitude"));
	FName AcceleratingDurationMagnitude(TEXT("AcceleratingRangedSpeedEffectMagnitude"));
}

UAccelerating::UAccelerating() {
	TypeId = EEnchantmentTypeID::Accelerating;
	ServerOnlyExecution = true;

	LevelMultiplier = &AcceleratingLevelMultiplier;
	MultiplierFormatter = valueformat::asPercentage;
	MobEffect = UAcceleratingMobGameplayEffect::StaticClass();
	EffectsForLevel = { UAcceleratingLevelOneGameplayEffect::StaticClass(), UAcceleratingLevelTwoGameplayEffect::StaticClass(), UAcceleratingLevelThreeGameplayEffect::StaticClass() };
}

float UAccelerating::GetSpeedLimit() {
	return SpeedLimit;
}

FText UAccelerating::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(resetDuration)));
}

void UAccelerating::OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key) {
	if (GetOwnerRole() != ROLE_Authority)
		return;

	float boostPerLevel = IsOwnerMob() ? MobSpeedIncrease : LevelMultiplier(Level);
	auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::RawMagnitude);
			
	TSubclassOf<UGameplayEffect> levelEffect = EffectsForLevel[Level - 1];
	TSubclassOf<UGameplayEffect> mobEffect = MobEffect;
	TSubclassOf<UGameplayEffect> effect = IsOwnerMob() ? mobEffect : levelEffect;
	if(effect){
		FGameplayEffectSpec spec(effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, effect, boostPerLevel));
		spec.SetSetByCallerMagnitude(::AcceleratingRangedSpeedEffectMagnitude,  boostPerLevel);
		spec.SetSetByCallerMagnitude(effects::DurationName, GetOwner()->IsA<APlayerCharacter>() ? resetDuration : resetDurationMob);
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}


UAcceleratingBaseGameplayEffect::UAcceleratingBaseGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo rangedDamageInfo;
	rangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();
	rangedDamageInfo.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(rangedDamageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.Accelerating")), 0.f, SpeedLimit);
	GameplayCues.Last().MagnitudeAttribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();
}

UAcceleratingLevelOneGameplayEffect::UAcceleratingLevelOneGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StackLimitCount = SpeedLimit / AcceleratingLevelMultiplier(1);
	Modifiers.Last().ModifierMagnitude = FScalableFloat(AcceleratingLevelMultiplier(1));
}

UAcceleratingLevelTwoGameplayEffect::UAcceleratingLevelTwoGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StackLimitCount = SpeedLimit / AcceleratingLevelMultiplier(2);
	Modifiers.Last().ModifierMagnitude = FScalableFloat(AcceleratingLevelMultiplier(2));
}

UAcceleratingLevelThreeGameplayEffect::UAcceleratingLevelThreeGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StackLimitCount = SpeedLimit / AcceleratingLevelMultiplier(3);
	Modifiers.Last().ModifierMagnitude = FScalableFloat(AcceleratingLevelMultiplier(3));
}


UAcceleratingMobGameplayEffect::UAcceleratingMobGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StackLimitCount = SpeedLimit / MobSpeedIncrease;
	Modifiers.Last().ModifierMagnitude = FScalableFloat(MobSpeedIncrease);
}