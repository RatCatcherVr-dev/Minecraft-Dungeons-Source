#include "Dungeons.h"
#include "FoodInstance.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/attributes/OxygenAttributeSet.h"
#include "game/abilities/effects/calculations/DamageModCalculations.h"
#include <AbilitySystemComponent.h>
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/HealingModCalculations.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

const float TICK_INTERVAL = 0.25f;

static float CalculateCoefficientPerTick(float duration, float coefficient) {
	return (coefficient / duration) * TICK_INTERVAL;
}

UFoodHealingGameplayEffect::UFoodHealingGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	bRequireModifierSuccessToTriggerCues = false;

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
}

UBreadHealingGameplayEffect::UBreadHealingGameplayEffect() {
	const float duration = 30.0f;
	const float baseCoefficient = 1.0f;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	durationMagnitude.Coefficient = duration;
	DurationMagnitude = durationMagnitude;

	FCustomCalculationBasedFloat healingMagnitude;
	healingMagnitude.CalculationClassMagnitude = UMaxHealthHealingModCalculation::StaticClass();
	healingMagnitude.Coefficient = CalculateCoefficientPerTick(duration, baseCoefficient);
	Period = TICK_INTERVAL;


	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healingMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Food.Bread.Heal"), 0, 400);

	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UAppleHealingGameplayEffect::UAppleHealingGameplayEffect() {
	const float duration = 3.0f;
	const float baseCoefficient = 0.2f;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	durationMagnitude.Coefficient = duration;
	DurationMagnitude = durationMagnitude;

	FCustomCalculationBasedFloat healingMagnitude;
	healingMagnitude.CalculationClassMagnitude = UMaxHealthHealingModCalculation::StaticClass();
	healingMagnitude.Coefficient = CalculateCoefficientPerTick(duration, baseCoefficient);
	Period = TICK_INTERVAL;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healingMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Food.Apple.Heal"), 0, 400);

	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UPorkHealingGameplayEffect::UPorkHealingGameplayEffect() {
	const float duration = 10.0f;
	const float baseCoefficient = 0.5f;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	durationMagnitude.Coefficient = duration;
	DurationMagnitude = durationMagnitude;

	FCustomCalculationBasedFloat healingMagnitude;
	healingMagnitude.CalculationClassMagnitude = UMaxHealthHealingModCalculation::StaticClass();
	healingMagnitude.Coefficient = CalculateCoefficientPerTick(duration, baseCoefficient);
	Period = TICK_INTERVAL;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healingMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Food.Pork.Heal"), 0, 400);

	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

	//UIData = 
}

USalmonHealingGameplayEffect::USalmonHealingGameplayEffect() {
	const float duration = 8.0f;
	const float baseCoefficient = 0.35f;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	durationMagnitude.Coefficient = duration;
	DurationMagnitude = durationMagnitude;

	FCustomCalculationBasedFloat healingMagnitude;
	healingMagnitude.CalculationClassMagnitude = UMaxHealthHealingModCalculation::StaticClass();
	healingMagnitude.Coefficient = CalculateCoefficientPerTick(duration, baseCoefficient);
	Period = TICK_INTERVAL;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healingMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Food.Salmon.Heal"), 0, 400);

	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();

}

USweetBerryHealingGameplayEffect::USweetBerryHealingGameplayEffect() {
	const float duration = 10.0f;
	const float baseCoefficient = 0.20f;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	durationMagnitude.Coefficient = duration;
	DurationMagnitude = durationMagnitude;

	FCustomCalculationBasedFloat healingMagnitude;
	healingMagnitude.CalculationClassMagnitude = UMaxHealthHealingModCalculation::StaticClass();
	healingMagnitude.Coefficient = CalculateCoefficientPerTick(duration, baseCoefficient);
	Period = TICK_INTERVAL;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healingMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Food.SweetBerry.Heal"), 0, 400);

	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

USweetBerrySwiftnessGameplayEffect::USweetBerrySwiftnessGameplayEffect() {
	const float duration = 5.0f;
	const float basePower = 1.5f;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	durationMagnitude.Coefficient = duration;
	DurationMagnitude = durationMagnitude;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();
	info.ModifierMagnitude = FScalableFloat(basePower);
	info.ModifierOp = EGameplayModOp::Type::Multiplicitive;
	Modifiers.Add(info);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Swiftness"), 0, 1);
}

UTropicalFishHealingGameplayEffect::UTropicalFishHealingGameplayEffect() {
	const float duration = 3.0f;
	const float baseCoefficient = 0.3f;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	durationMagnitude.Coefficient = duration;
	DurationMagnitude = durationMagnitude;
	Period = TICK_INTERVAL;

	FAttributeBasedFloat healing;
	healing.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;
	healing.Coefficient = CalculateCoefficientPerTick(duration, baseCoefficient);
	healing.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	healing.BackingAttribute.bSnapshot = true;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healing;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Food.TropicalFish.Heal"), 0, 400);

	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

UTropicalFishOxygenGameplayEffect::UTropicalFishOxygenGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FAttributeBasedFloat oxygen;
	oxygen.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;
	oxygen.Coefficient = 0.1f;
	oxygen.BackingAttribute.AttributeToCapture = UOxygenAttributeSet::MaxOxygenAttribute();
	oxygen.BackingAttribute.bSnapshot = true;

	FGameplayModifierInfo oxygenInfo;
	oxygenInfo.Attribute = UOxygenAttributeSet::OxygenAttribute();
	oxygenInfo.ModifierMagnitude = oxygen;
	oxygenInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(oxygenInfo);
}

UChorusFruitHealingGameplayEffect::UChorusFruitHealingGameplayEffect()
{
	const float duration = 1.0f;
	const float baseCoefficient = 0.30f;

	DurationMagnitude = FScalableFloat(duration);

	FCustomCalculationBasedFloat healingMagnitude;
	healingMagnitude.CalculationClassMagnitude = UMaxHealthHealingModCalculation::StaticClass();
	healingMagnitude.Coefficient = CalculateCoefficientPerTick(duration, baseCoefficient);
	Period = TICK_INTERVAL;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healingMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Food.ChorusFruit.Heal"), 0, 400);

	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}

void AFoodInstance::Activate(const FPredictionKey& predictionKey) {
	if (HasAuthority()) {
		if (const auto owner = Cast<ABaseCharacter>(GetOwner())) {
			if (const auto abilitySystem = owner->GetAbilitySystemComponent()) {
				FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
				auto foodEffect = FoodEffect->GetDefaultObject<UFoodHealingGameplayEffect>();

				FGameplayEffectSpec spec(foodEffect, abilitySystem->MakeEffectContext(), ItemPower);			
				spec.GetContext().AddInstigator(GetOwner(), this);
				abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);

				for (auto effect : AdditionalEffects) {
					FParameterFilterContextWindow AdditionalParamWindow(EGameplayCueParametersField::EMPTY);
					auto effectObject = effect->GetDefaultObject<UFoodHealingGameplayEffect>();

					FGameplayEffectSpec loopSpec(effectObject, abilitySystem->MakeEffectContext(), ItemPower);
					loopSpec.GetContext().AddInstigator(GetOwner(), this);
					abilitySystem->ApplyGameplayEffectSpecToSelf(loopSpec, predictionKey);
				}
			}
		}
	}
	Super::Activate(predictionKey);
}
