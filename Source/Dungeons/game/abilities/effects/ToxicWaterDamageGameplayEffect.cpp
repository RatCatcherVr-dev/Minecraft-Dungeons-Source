#include "Dungeons.h"
#include "ToxicWaterDamageGameplayEffect.h"
#include "executions/DamageExecutionCalculation.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include <GameplayEffect.h>
#include "game/actor/character/BaseCharacter.h"
#include "calculations/DamageModCalculations.h"
#include "game/component/HealthComponent.h"

UToxicWaterDamageGameplayEffect::UToxicWaterDamageGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bSuppressStackingCues = true;
	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = false;
	Period = 0.2f;


	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UWorldDamageModCalculation::StaticClass();
	healthMagnitude.Coefficient = Period.GetValueAtLevel(1) * DamagePerSecond;	

	FGameplayModifierInfo healthModifier;
	healthModifier.ModifierOp = EGameplayModOp::Additive;
	healthModifier.ModifierMagnitude = healthMagnitude;	
	healthModifier.Attribute = UHealthAttributeSet::HealthAttribute();
	Modifiers.Add(healthModifier);

	const auto toxicWaterDamageTag = damageTag::toxicWater();
	InheritableGameplayEffectTags.AddTag(toxicWaterDamageTag);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();	
	damage.PassedInTags.AddTag(toxicWaterDamageTag);
	Executions.Add(std::move(damage));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Damage.Medium"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}
