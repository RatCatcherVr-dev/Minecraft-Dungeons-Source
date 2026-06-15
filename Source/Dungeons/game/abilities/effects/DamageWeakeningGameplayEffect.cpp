#include "Dungeons.h"
#include "DamageWeakeningGameplayEffect.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "requirements/BaseGameplayEffectApplicationRequirement.h"
#include "calculations/StatusModCalculations.h"


const FName UDamageWeakeningGameplayEffect::DurationName = FName("Duration");
const FName UDamageWeakeningGameplayEffect::DamageDealtDividerName = FName("MeleeDamageDividerDealt");

UDamageWeakeningGameplayEffect::UDamageWeakeningGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
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

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = DamageDealtDividerName;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(dealDamageInfo);

	FGameplayModifierInfo dealRangedDamageInfo;
	dealRangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();
	dealRangedDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealRangedDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(dealRangedDamageInfo);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Weaken.Attack")), 0, 1);
}
