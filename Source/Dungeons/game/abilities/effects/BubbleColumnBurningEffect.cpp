#include "BubbleColumnBurningEffect.h"
#include "Dungeons.h"
#include "executions/DamageExecutionCalculation.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include <GameplayEffect.h>
#include "calculations/DamageModCalculations.h"
#include "game/component/HealthComponent.h"

UBubbleColumnBurningEffect::UBubbleColumnBurningEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::None;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::NeverReset;
	bSuppressStackingCues = true;
	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = false;
	Period = 0.2f;

	FAttributeBasedFloat healthMagnitude;
	healthMagnitude.AttributeCalculationType = EAttributeBasedFloatCalculationType::AttributeMagnitude;
	healthMagnitude.Coefficient = -1.f * DamagePercentPerSecond * Period.GetValue();
	healthMagnitude.BackingAttribute.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	healthMagnitude.BackingAttribute.AttributeToCapture = UHealthAttributeSet::MaxHealthAttribute();
	healthMagnitude.BackingAttribute.bSnapshot = false;

	FGameplayModifierInfo healthModifier;
	healthModifier.ModifierOp = EGameplayModOp::Additive;
	healthModifier.ModifierMagnitude = healthMagnitude;
	healthModifier.Attribute = UHealthAttributeSet::HealthAttribute();
	Modifiers.Add(healthModifier);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.BubbleColumnBurning"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.BubbleColumn.Burning"));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.BubbleColumn.Burning"));
	InheritableGameplayEffectTags.AddTag(damageTag::damageFriends());
}