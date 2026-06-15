#include "Dungeons.h"
#include "BaseItemDamageGameplayEffect.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/executions/LifestealExecution.h"
#include "calculations/DamageModCalculations.h"
#include "executions/DamageExecutionCalculation.h"

UBaseItemDamageGameplayEffect::UBaseItemDamageGameplayEffect() {
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UItemDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Item")));

	FGameplayEffectExecutionDefinition lifesteal;
	lifesteal.CalculationClass = UItemLifestealExecution::StaticClass();
	Executions.Add(std::move(lifesteal));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));
}
