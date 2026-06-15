#include "Dungeons.h"
#include "HealthPotionInstance.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/HealingModCalculations.h"

bool AHealthPotionInstance::CanActivate() const {
	return Super::CanActivate() && (GetOwner() && GetOwner()->FindComponentByClass<UHealthComponent>() != nullptr);
}

void AHealthPotionInstance::Activate(const FPredictionKey& predictionKey) {
	auto owner = Cast<ABaseCharacter>(GetOwner());

	FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
	UAbilitySystemComponent* abilitySystem = owner->GetAbilitySystemComponent();
	FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UHealthPotionGameplayEffect>(abilitySystem, 1.f);
	abilitySystem->ApplyGameplayEffectSpecToSelf(spec, predictionKey);
	spec.GetContext().AddInstigator(GetOwner(), this);
	spec.GetContext().AddOrigin(GetOwner()->GetActorLocation());
	Super::Activate(predictionKey);
}


UHealthPotionGameplayEffect::UHealthPotionGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FCustomCalculationBasedFloat healingMagnitude;
	healingMagnitude.CalculationClassMagnitude = UMaxHealthHealingModCalculation::StaticClass();
	healingMagnitude.Coefficient = 0.75f;

	FGameplayModifierInfo healthInfo;
	healthInfo.Attribute = UHealthAttributeSet::HealthAttribute();
	healthInfo.ModifierMagnitude = healingMagnitude;
	healthInfo.ModifierOp = EGameplayModOp::Type::Additive;
	Modifiers.Add(healthInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Potion.Health"), 0, 400);
	GameplayCues.Last().MagnitudeAttribute = UHealthAttributeSet::HealthAttribute();
	bRequireModifierSuccessToTriggerCues = false;
}