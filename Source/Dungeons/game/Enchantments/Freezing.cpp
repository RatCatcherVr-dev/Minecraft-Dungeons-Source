#include "Dungeons.h"
#include "Freezing.h"

#include "AbilitySystemComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <GameplayEffect.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/effects/calculations/ResistanceModCalculations.h"
#include "util/CharacterQuery.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

namespace FreezingMagnitudes {
	FName FreezingSlowingEffectDivider = "FreezingSlowingEffectMagnitude";
	FName FreezingDurationEffectMagnitude = "FreezingDurationEffectMagnitude";
}

UFreezingEnchantmentBase::UFreezingEnchantmentBase() {
	Effect = UFreezingGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::Freezing;
	PredictiveExecution = true;

	LevelMultiplier = [this](int level) -> float {
		return SlowAmountBase - SlowAmountPerLevel * (level - 1);
	};
	MultiplierFormatter = valueformat::asRelativeMultiplierPercentageChange;
}

FText UFreezingEnchantmentBase::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asForRoundedWordSecond(FreezeTime)));
}

void UFreezingEnchantmentBase::ApplyEffectToTarget(AActor* ToTarget, FPredictionKey PredictionKey) {
	auto characterOwner = GetCharacterOwner();
	auto target = Cast<ABaseCharacter>(ToTarget);
	
	if (!target || !characterquery::is::targetable(target) || !actorquery::is::alive(target)) {
		return;
	}

	if(GetOwnerRole() == ROLE_Authority) BroadcastEnchantmentTriggeredEvent();

	auto abilitySystem = characterOwner->GetAbilitySystemComponent();

	const float SlowMultiplier = LevelMultiplier(Level);
	const float SlowDivider = 1 / SlowMultiplier;

	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, Effect, Level);
	spec.SetSetByCallerMagnitude(USlowMultiplicativeResistanceModCalculation::SetByCallerKey, SlowDivider);
	spec.SetSetByCallerMagnitude(effects::DurationName, FreezeTime);
	abilitySystem->ApplyGameplayEffectSpecToTarget(spec, target->GetAbilitySystemComponent(), PredictionKey);
}

void UFreezing::OnAfterDealtMeleeDamage(float Damage, bool missedAttack, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream, FSharedPredictionContext context) {
	ApplyEffectToTarget(toWhat, context.GetKey());
}

void UFreezingAoe::OnAfterDealtAoeDamage(AActor* attackTarget, FRandomStream& randStream, FSharedPredictionContext context) {
	ApplyEffectToTarget(attackTarget, context.GetKey());
}

void UFreezingRanged::OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) {
	ApplyEffectToTarget(toWhat);
}

UFreezingGameplayEffect::UFreezingGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	// #D11.CM - Set resistance calculation

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
	bSuppressStackingCues = true;

	// Get our unifying effect * resistance magnitude
	FCustomCalculationBasedFloat effectResistanceMagnitude;
	effectResistanceMagnitude.CalculationClassMagnitude = USlowMultiplicativeResistanceModCalculation::StaticClass();

	// Apply Move Speed Effect
	FGameplayModifierInfo moveSpeedInfo;
	moveSpeedInfo.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	moveSpeedInfo.ModifierMagnitude = effectResistanceMagnitude;
	moveSpeedInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(moveSpeedInfo);

	// Apply Melee Speed Effect
	FGameplayModifierInfo meleeSpeedInfo;
	meleeSpeedInfo.Attribute = UMeleeAttributeSet::MeleeAttackSpeedMultiplierAttribute();
	meleeSpeedInfo.ModifierMagnitude = effectResistanceMagnitude;
	meleeSpeedInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(meleeSpeedInfo);

	// Apply Ranged Speed Effect
	FGameplayModifierInfo rangedSpeedInfo;
	rangedSpeedInfo.Attribute = URangedAttributeSet::RangedAttackSpeedMultiplierAttribute();
	rangedSpeedInfo.ModifierMagnitude = effectResistanceMagnitude;
	rangedSpeedInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(rangedSpeedInfo);

	// Apply Tags
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Negative"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Slow"));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.Freezing"), 0, 1);
}

