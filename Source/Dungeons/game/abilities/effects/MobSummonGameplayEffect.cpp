#include "MobSummonGameplayEffect.h"
#include "calculations/ItemPowerModCalculations.h"
#include "game/abilities/attributes/DifficultyAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

UMobSummonGameplayEffect::UMobSummonGameplayEffect(const FObjectInitializer& ObjectInitializer) {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo maxHealthInfo;
	maxHealthInfo.Attribute = UHealthAttributeSet::MaxHealthAttribute();

	FCustomCalculationBasedFloat increasedMaxHealthMagnitude;
	increasedMaxHealthMagnitude.CalculationClassMagnitude = UItemPowerHealthModCalculation::StaticClass();

	maxHealthInfo.ModifierMagnitude = increasedMaxHealthMagnitude;
	maxHealthInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(maxHealthInfo);

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UDifficultyAttributeSet::DifficultyDealDamageMultiplierAttribute();

	FCustomCalculationBasedFloat dealDamageMagnitude;
	dealDamageMagnitude.CalculationClassMagnitude = UItemPowerMeleeDamageModCalculation::StaticClass();

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(dealDamageInfo);
}