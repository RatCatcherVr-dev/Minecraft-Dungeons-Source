#include "Dungeons.h"
#include "WeakenGameplayEffect.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"


const FName UWeakenGameplayEffect::DurationName = FName("Duration");
const FName UWeakenGameplayEffect::DamageTakenName = FName("DamageTaken");
const FName UWeakenGameplayEffect::DamageDealtName = FName("MeleeDamageDealt");

UWeakenGameplayEffect::UWeakenGameplayEffect(const FObjectInitializer& ObjectInitializer)
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

	FGameplayModifierInfo takeDamageInfo;
	takeDamageInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();

	FSetByCallerFloat takeDamageMagnitude;
	takeDamageMagnitude.DataName = DamageTakenName;

	takeDamageInfo.ModifierMagnitude = takeDamageMagnitude;
	takeDamageInfo.ModifierOp = EGameplayModOp::Type::Multiplicitive;

	Modifiers.Add(takeDamageInfo);

	FGameplayModifierInfo dealDamageInfo;
	dealDamageInfo.Attribute = UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute();

	FSetByCallerFloat dealDamageMagnitude;
	dealDamageMagnitude.DataName = DamageDealtName;

	dealDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(dealDamageInfo);

	FGameplayModifierInfo dealRangedDamageInfo;
	dealRangedDamageInfo.Attribute = URangedAttributeSet::RangedAttackDamageMultiplerAttribute();
	dealRangedDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealRangedDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(dealRangedDamageInfo);

	// D11.DB - Start - Include item damage.
	FGameplayModifierInfo dealItemDamageInfo;
	dealItemDamageInfo.Attribute = UItemAttributeSet::ItemDamageMultiplierAttribute();
	dealItemDamageInfo.ModifierMagnitude = dealDamageMagnitude;
	dealItemDamageInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(dealItemDamageInfo);
	// D11.DB - End

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Weaken")), 0, 1);
}
