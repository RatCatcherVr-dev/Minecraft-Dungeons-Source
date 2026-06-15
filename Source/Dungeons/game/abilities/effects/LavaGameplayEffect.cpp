#include "Dungeons.h"
#include "executions/DamageExecutionCalculation.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "LavaGameplayEffect.h"
#include <GameplayEffect.h>
#include "calculations/DamageModCalculations.h"
#include "game/component/HealthComponent.h"
#include "Object.h"
#include "calculations/ResistanceModCalculations.h"

ULavaGameplayEffect::ULavaGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bSuppressStackingCues = true;
	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = false;
	Period = 0.2f;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UEnvironmentalProtectionModCalculation::StaticClass();
	healthMagnitude.Coefficient = Period.GetValueAtLevel(1) * DamagePerSecond;

	FGameplayModifierInfo healthModifier;
	healthModifier.ModifierOp = EGameplayModOp::Additive;
	healthModifier.ModifierMagnitude = healthMagnitude;
	healthModifier.Attribute = UHealthAttributeSet::HealthAttribute();
	Modifiers.Add(healthModifier);

	const auto lavaDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Lava"));
	const auto environmentalDamageTag = damageTag::environmental();
	InheritableGameplayEffectTags.AddTag(lavaDamageTag);
	InheritableGameplayEffectTags.AddTag(environmentalDamageTag);
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Fire")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	damage.PassedInTags.AddTag(lavaDamageTag);
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Environmental")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Lava"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Fire.Lava"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Fire.Lava"));
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}
