#include "Dungeons.h"
#include "LifestealExecution.h"
#include <AbilitySystemComponent.h>
#include "game/component/HealthComponent.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/effects/calculations/HealingModCalculations.h"

ULifestealGameplayEffect::ULifestealGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healthMagnitude;
	healthMagnitude.CalculationClassMagnitude = UHealingModCalculation::StaticClass();

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healthMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.LifeSteal"), 0, 1);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
}


UBaseLifestealExecution::UBaseLifestealExecution() : TargetHealthCapture(UHealthAttributeSet::HealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, true) {
	RelevantAttributesToCapture.Emplace(TargetHealthCapture);
}

void UBaseLifestealExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const {
	const auto& spec = ExecutionParams.GetOwningSpec();
	const auto healthAttribute = spec.GetModifiedAttribute(UHealthAttributeSet::HealthAttribute());
	const auto damage = healthAttribute && healthAttribute->TotalMagnitude < 0;
	const auto sourceCharacter = Cast<ABaseCharacter>(spec.GetContext().GetInstigator());

	if (damage && sourceCharacter) {
		auto damageDealerAbilitySystem = sourceCharacter->GetAbilitySystemComponent();
		const auto totalDamage = -healthAttribute->TotalMagnitude;
		auto lifeStealMultiplier = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(LifeStealCapture, FAggregatorEvaluateParameters(), lifeStealMultiplier);
		if (lifeStealMultiplier > 0.0f) {
			auto targetHealth = 0.f;
			ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(TargetHealthCapture, FAggregatorEvaluateParameters(), targetHealth);

			const auto lifestealMagnitude = lifeStealMultiplier * FMath::Min(targetHealth, totalDamage);
			const auto healthComponent = sourceCharacter->GetHealthComponent();
			if (lifestealMagnitude > 0.0f && healthComponent && !healthComponent->IsHealthMaxed()) {
				const auto lifestealSpec = effects::CreateGameplayEffectSpec<ULifestealGameplayEffect>(damageDealerAbilitySystem, effects::HealthName, lifestealMagnitude, sourceCharacter, spec.GetContext().GetEffectCauser(), sourceCharacter->GetActorLocation(), 1.f);
				damageDealerAbilitySystem->ApplyGameplayEffectSpecToSelf(lifestealSpec);
			}
		}
	}
}

UMeleeLifestealExecution::UMeleeLifestealExecution() {
	LifeStealCapture = { UMeleeAttributeSet::MeleeAttackLifeStealAmountAttribute(), EGameplayEffectAttributeCaptureSource::Source, true };
	RelevantAttributesToCapture.Emplace(LifeStealCapture);
}

URangedLifestealExecution::URangedLifestealExecution() {
	LifeStealCapture = { URangedAttributeSet::RangedAttackLifeStealAmountAttribute(), EGameplayEffectAttributeCaptureSource::Source, true };
	RelevantAttributesToCapture.Emplace(LifeStealCapture);
}

UItemLifestealExecution::UItemLifestealExecution() {
	LifeStealCapture = { UItemAttributeSet::ItemLifeStealAmountAttribute(), EGameplayEffectAttributeCaptureSource::Source, true };
	RelevantAttributesToCapture.Emplace(LifeStealCapture);
}