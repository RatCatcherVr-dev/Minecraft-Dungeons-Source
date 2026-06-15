#include "Dungeons.h"
#include "DamageSelfGameplayEffect.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "calculations/DamageModCalculations.h"
#include "executions/DamageExecutionCalculation.h"
#include "game/component/HealthComponent.h"

UDamageSelfGameplayEffect::UDamageSelfGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UDamageModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage")));

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}
