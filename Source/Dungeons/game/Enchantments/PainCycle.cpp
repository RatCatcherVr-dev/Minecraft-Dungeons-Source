// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "PainCycle.h"
#include "game/component/MeleeAttackComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <AbilitySystemComponent.h>

#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "TimerManager.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/effects/calculations/HealingModCalculations.h"


namespace {
	FName PainCycleBuffDamageEffectMagnitude(TEXT("PainCycleBuffDamageEffectMagnitude"));
}

const FName UPainCycleHealthDrainModCalculation::DrainKey(TEXT("PainCycleDrainKey"));


UPainCycleHealthDrainModCalculation::UPainCycleHealthDrainModCalculation() :
	HealthAttributeCapture(UHealthAttributeSet::HealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, true),
	MaxHealthAttributeCapture(UHealthAttributeSet::MaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, true)
{
	RelevantAttributesToCapture.Add(HealthAttributeCapture);
	RelevantAttributesToCapture.Add(MaxHealthAttributeCapture);
}

float UPainCycleHealthDrainModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float MaxHealthPercentage = Spec.GetSetByCallerMagnitude(DrainKey, true, 1.f);

	float maxHealth = 10, currentHealth = 10;
	GetCapturedAttributeMagnitude(MaxHealthAttributeCapture, Spec, FAggregatorEvaluateParameters(), maxHealth);
	GetCapturedAttributeMagnitude(HealthAttributeCapture, Spec, FAggregatorEvaluateParameters(), currentHealth);

	float intendedDamage = maxHealth * MaxHealthPercentage;
	float allowedDamage = FMath::Max(currentHealth - 1.0f, 0.0f);  // 0.5f
	float actualDamage = -FMath::Min(intendedDamage, allowedDamage);
	return actualDamage;
}


//////////////////////////////////////////////////////////////////////////////////

UPainCycleStackerGameplayEffect::UPainCycleStackerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;
	bClearStackOnOverflow = true;
	bDenyOverflowApplication = true;
	StackLimitCount = MaximumPainCycleStacks; // overflows at 11, but feels more intuitive regardless

	OverflowEffects.Add(UPainCycleBuffGameplayEffect::StaticClass());

	//Note, if stacks reset on death, you need to handle resetting the stack count in the enchant on death as well. If not, uncomment this line...
	//InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("PersistAfterDeath"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PainCycle.Stacker"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PainCycle.Stacker"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.PainCycle.Stacker"), 1, 1);
}


//////////////////////////////////////////////////////////////////////////////////

UPainCycleDrainGameplayEffect::UPainCycleDrainGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthDrainMagnitude;
	healthDrainMagnitude.CalculationClassMagnitude = UPainCycleHealthDrainModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthDrainMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
	InheritableGameplayEffectTags.AddTag(damageTag::weak());
}


UPainCycleBuffGameplayEffect::UPainCycleBuffGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = ::PainCycleBuffDamageEffectMagnitude;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PainCycle.Buff"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.PainCycle.Buff"));
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Melee.Buff.PainCycle"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.PainCycle.Buff"), 1, 1);
}

//////////////////////////////////////////////////////////////////////////////////

UPainCycle::UPainCycle() {
	TypeId = EEnchantmentTypeID::PainCycle;
	TargetEffect = UPainCycleStackerGameplayEffect::StaticClass();
	DrainEffect = UPainCycleDrainGameplayEffect::StaticClass();

	LevelMultiplier = [this](int level) -> float {
		return MinimumBonusDamageMultiplier + ((level-1) * BonusDamageMultiplierPerExtraLevel);
	};
	MultiplierFormatter = valueformat::asRoundedMultiple;
}

void UPainCycle::OnEnd() {
	GetAbilitySystemComponent()->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.PainCycle")));
}

FText UPainCycle::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), 
		FText::FromString(valueformat::asConstant(UPainCycleStackerGameplayEffect::StaticClass()->GetDefaultObject<UPainCycleStackerGameplayEffect>()->MaximumPainCycleStacks + 1)) // @todo: fix this later
	);
}

void UPainCycle::OnBeforeDealtMeleeDamage(float& outPushbackMultiplier, FGameplayEffectSpec& mutableSpec, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FSharedPredictionContext context) {
	auto abilitySystem = GetAbilitySystemComponent();
	if(abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.PainCycle.Buff")))
	{
		effects::GetDungeonsContextFromSpec(mutableSpec)->AdditionalCues.Add(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Hit.PainCycle.Melee")));

		// do NOT remove the buff here!
		// OnAfterMeleeAttack() will handle that, so that it knows NOT to stack the current drain mechanic if that's what we want for later.
	}
}

void UPainCycle::OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) {
	if(hitCount <= 0) {
		return;
	}
		
	auto abilitySystem = GetAbilitySystemComponent();
	// if the effect is active, it would've done its job in OnBeforeDealtMeleeDamage().
	// just remove it.  inhibit stacking of the drain if necessary
	if (abilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("StatusEffect.PainCycle.Buff")))
	{
		// remove the effects first
		abilitySystem->RemoveActiveEffectsWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag("StatusEffect.PainCycle.Buff")));
	}
	else {
		// update the stacker stack
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, TargetEffect, Level);
		spec.SetSetByCallerMagnitude(::PainCycleBuffDamageEffectMagnitude, LevelMultiplier(Level));
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	
		FGameplayEffectSpec drainSpec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, DrainEffect, Level);
		drainSpec.SetSetByCallerMagnitude(UPainCycleHealthDrainModCalculation::DrainKey, DrainMagnitude);
		abilitySystem->ApplyGameplayEffectSpecToSelf(drainSpec);
	}
}

