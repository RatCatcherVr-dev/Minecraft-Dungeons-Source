#include "Dungeons.h"
#include "HealingModCalculations.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/DifficultyAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/abilities/attributes/HealthAttributeSet.h"


UPositiveHealthModCalculation::UPositiveHealthModCalculation()
{
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UPositiveHealthModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return Spec.GetSetByCallerMagnitude(effects::HealthName, false, 1.f);
}





UHealingModCalculation::UHealingModCalculation()
	: PerformHealingAttributeCapture(UHealthAttributeSet::ReceiveHealingMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Target, true)
{
	RelevantAttributesToCapture.Emplace(PerformHealingAttributeCapture);
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UHealingModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float performHealingMultiplier = 1.f;
	GetCapturedAttributeMagnitude(PerformHealingAttributeCapture, Spec, FAggregatorEvaluateParameters(), performHealingMultiplier);
	return Super::CalculateBaseMagnitude_Implementation(Spec) * performHealingMultiplier;
}



UMaxHealthHealingModCalculation::UMaxHealthHealingModCalculation()
	: MaxHealthAttributeCapture(UHealthAttributeSet::MaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Target, true)
{
	RelevantAttributesToCapture.Emplace(MaxHealthAttributeCapture);
}

float UMaxHealthHealingModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float maxHealth = 1.f;
	GetCapturedAttributeMagnitude(MaxHealthAttributeCapture, Spec, FAggregatorEvaluateParameters(), maxHealth);
	return Super::CalculateBaseMagnitude_Implementation(Spec) * maxHealth;
}



UDifficultyHealingModCalculation::UDifficultyHealingModCalculation()
	: PerformHealingAttributeCapture(UDifficultyAttributeSet::DifficultyPerformHealingMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(PerformHealingAttributeCapture);
}

float UDifficultyHealingModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {	
	float performHealingMultiplier = 1.f;
	GetCapturedAttributeMagnitude(PerformHealingAttributeCapture, Spec, FAggregatorEvaluateParameters(), performHealingMultiplier);

	const float healing = Super::CalculateBaseMagnitude_Implementation(Spec);
	return healing * performHealingMultiplier;
}








UMeleeItemPowerOnlyModHealingCalculation::UMeleeItemPowerOnlyModHealingCalculation()
	: MeleeItemPowerFactorCapture(UMeleeAttributeSet::MeleeHealItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	RelevantAttributesToCapture.Emplace(MeleeItemPowerFactorCapture);
}

float UMeleeItemPowerOnlyModHealingCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const float healing = Super::CalculateBaseMagnitude_Implementation(Spec);

	float meleeHealItemPowerFactor = 1.f;
	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(MeleeItemPowerFactorCapture, Spec, params, meleeHealItemPowerFactor);

	return healing * meleeHealItemPowerFactor;
}

URangedItemPowerOnlyModHealingCalculation::URangedItemPowerOnlyModHealingCalculation()
	: RangedItemPowerFactorCapture(URangedAttributeSet::RangedHealItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	RelevantAttributesToCapture.Emplace(RangedItemPowerFactorCapture);
}

float URangedItemPowerOnlyModHealingCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const float healing = Super::CalculateBaseMagnitude_Implementation(Spec);

	float rangedHealItemPowerFactor = 1.f;
	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(RangedItemPowerFactorCapture, Spec, params, rangedHealItemPowerFactor);

	return healing * rangedHealItemPowerFactor;
}

UArmorItemPowerOnlyModHealingCalculation::UArmorItemPowerOnlyModHealingCalculation()
	: ArmorItemPowerFactorCapture(UHealthAttributeSet::ArmorHealingItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	RelevantAttributesToCapture.Emplace(ArmorItemPowerFactorCapture);
}

float UArmorItemPowerOnlyModHealingCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const float healing = Super::CalculateBaseMagnitude_Implementation(Spec);

	float armorHealItemPowerFactor = 1.f;
	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(ArmorItemPowerFactorCapture, Spec, params, armorHealItemPowerFactor);

	return healing * armorHealItemPowerFactor;
}