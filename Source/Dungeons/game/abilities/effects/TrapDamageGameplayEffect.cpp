#include "Dungeons.h"
#include "TrapDamageGameplayEffect.h"
#include "calculations/ResistanceModCalculations.h"
#include "executions/DamageExecutionCalculation.h"
#include "executions/LifestealExecution.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "calculations/TrapDamageModCalculations.h"

UTrapDamageGameplayEffect::UTrapDamageGameplayEffect() 
{
	Modifiers.Empty();

	FGameplayModifierInfo healthInfo;
	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UTrapDamageModCalculation::StaticClass();

	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	FGameplayEffectCue HealthGameplayCue;
	HealthGameplayCue.MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	HealthGameplayCue.MinLevel = 0.f;
	HealthGameplayCue.MaxLevel = 1.f;
	HealthGameplayCue.GameplayCueTags.AddTag(FGameplayTag::RequestGameplayTag("GameplayCue.Damage"));
	GameplayCues.Add(HealthGameplayCue);

	InheritableGameplayEffectTags.AddTag(damageTag::trap());
}
