#include "Dungeons.h"
#include "DamageModCalculations.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/difficulty/DifficultyStats.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/attributes/ItemAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/DifficultyAttributeSet.h"
#include "game/abilities/attributes/AffectorAttributeSet.h"
#include "game/abilities/attributes/DamageAttributeSet.h"
#include "game/item/power/ItemPowerStats.h"

UNegativeHealthModCalculation::UNegativeHealthModCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UNegativeHealthModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return Spec.GetSetByCallerMagnitude(effects::HealthName, false, -1.f);
}


UDamageModCalculation::UDamageModCalculation()
	: TakeDamageAttributeCapture(UHealthAttributeSet::TakeDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(TakeDamageAttributeCapture);
}

float UDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float TakeDamageMultiplier = 1.f;
	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(TakeDamageAttributeCapture, Spec, params, TakeDamageMultiplier);

	const float damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	return (damage + PremultiplyAdd(Spec, params)) * TakeDamageMultiplier;
}



UDamageModSourceCalculation::UDamageModSourceCalculation()
	: GiveDamageMultiplierAttribute(UDamageAttributeSet::DamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	RelevantAttributesToCapture.Emplace(GiveDamageMultiplierAttribute);
}

float UDamageModSourceCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	FGameplayTagContainer tags;
	Spec.GetAllAssetTags(tags);
	FAggregatorEvaluateParameters damageParams;
	damageParams.SourceTags = &tags;

	float GiveDamageMultiplier = 1.f;
	GetCapturedAttributeMagnitude(GiveDamageMultiplierAttribute, Spec, damageParams, GiveDamageMultiplier);

	return Super::CalculateBaseMagnitude_Implementation(Spec) * GiveDamageMultiplier;
}


UAffectorDamageModCalculation::UAffectorDamageModCalculation()
	: DealDamageAttributeCapture(UAffectorAttributeSet::DealDamageMulAttribute(), EGameplayEffectAttributeCaptureSource::Source, true) {
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(DealDamageAttributeCapture);
}

float UAffectorDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float affectorDamageMultiplier = 1.f;
	GetCapturedAttributeMagnitude(DealDamageAttributeCapture, Spec, FAggregatorEvaluateParameters(), affectorDamageMultiplier);
	return Super::CalculateBaseMagnitude_Implementation(Spec) * affectorDamageMultiplier;
}


UDifficultyDamageModCalculation::UDifficultyDamageModCalculation()
	: DealDamageAttributeCapture(UDifficultyAttributeSet::DifficultyDealDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(DealDamageAttributeCapture);
}

float UDifficultyDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float DealDamageMultiplier = 1.f;
	GetCapturedAttributeMagnitude(DealDamageAttributeCapture, Spec, FAggregatorEvaluateParameters(), DealDamageMultiplier);

	const float damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	return damage * DealDamageMultiplier;
}

UDifficultyTargetDamageModCalculation::UDifficultyTargetDamageModCalculation()
	: DealDamageAttributeCapture(UDifficultyAttributeSet::DifficultyDealDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Target, true)
{
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(DealDamageAttributeCapture);
}

float UDifficultyTargetDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float DealDamageMultiplier = 1.f;
	GetCapturedAttributeMagnitude(DealDamageAttributeCapture, Spec, FAggregatorEvaluateParameters(), DealDamageMultiplier);

	const float damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	return damage * DealDamageMultiplier;
}


UMeleeModDamageCalculation::UMeleeModDamageCalculation()
	: MeleeDamageMultiplierCapture(UMeleeAttributeSet::MeleeAttackDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, MeleeItemPowerFactorCapture(UMeleeAttributeSet::MeleeAttackItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, MeleeDamageIncreaseCapture(UMeleeAttributeSet::MeleeAttackDamageIncreaseAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, TakeMeleeDamageAttributeCapture(UHealthAttributeSet::TakeMeleeDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	RelevantAttributesToCapture.Emplace(MeleeDamageMultiplierCapture);
	RelevantAttributesToCapture.Emplace(MeleeItemPowerFactorCapture);
	RelevantAttributesToCapture.Emplace(MeleeDamageIncreaseCapture);
	RelevantAttributesToCapture.Emplace(TakeMeleeDamageAttributeCapture);
}

float UMeleeModDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const auto difficultyScaledDamage = Super::CalculateBaseMagnitude_Implementation(Spec);

	auto meleeDamageMultiplier = 1.f;
	auto meleeItemPowerFactor = 1.f;
	auto targetMeleeDamageMultiplier = 1.f;

	const FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(MeleeDamageMultiplierCapture, Spec, params, meleeDamageMultiplier);
	GetCapturedAttributeMagnitude(MeleeItemPowerFactorCapture, Spec, params, meleeItemPowerFactor);
	GetCapturedAttributeMagnitude(TakeMeleeDamageAttributeCapture, Spec, params, targetMeleeDamageMultiplier);

	return difficultyScaledDamage * meleeItemPowerFactor * meleeDamageMultiplier * targetMeleeDamageMultiplier;
}

float UMeleeModDamageCalculation::PremultiplyAdd(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& params) const {
	auto addition = 0.f;
	GetCapturedAttributeMagnitude(MeleeDamageIncreaseCapture, Spec, params, addition);
	return -addition;
}


UItemDamageModCalculation::UItemDamageModCalculation() :ItemDamageModCapture(UItemAttributeSet::ItemDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true) {
	RelevantAttributesToCapture.Emplace(ItemDamageModCapture);
}

float UItemDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const float difficultyScaledDamage = Super::CalculateBaseMagnitude_Implementation(Spec);
	float itemDamageMultiplier = 1.f;
	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(ItemDamageModCapture, Spec, FAggregatorEvaluateParameters(), itemDamageMultiplier);

	return difficultyScaledDamage * itemDamageMultiplier;
}


URangedDamageModCalculation::URangedDamageModCalculation()
	: RangedDamageModCapture(URangedAttributeSet::RangedAttackDamageMultiplerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, RangedItemPowerFactorCapture(URangedAttributeSet::RangedAttackItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, RangedDamageIncreaseCapture(URangedAttributeSet::RangedAttackDamageIncreaseAttribute(), EGameplayEffectAttributeCaptureSource::Source, false)
	, TakeRangeDamageAttributeCapture(UHealthAttributeSet::TakeRangeDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Target, false)
{
	RelevantAttributesToCapture.Emplace(RangedDamageModCapture);
	RelevantAttributesToCapture.Emplace(RangedItemPowerFactorCapture);
	RelevantAttributesToCapture.Emplace(RangedDamageIncreaseCapture);
	RelevantAttributesToCapture.Emplace(TakeRangeDamageAttributeCapture);
}

float URangedDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const float difficultyScaledDamage = Super::CalculateBaseMagnitude_Implementation(Spec);
	float rangedDamageMultiplier = 1.f;
	float rangedItemPowerFactor = 1.f;
	auto targetRangeDamageMultiplier = 1.f;

	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(RangedDamageModCapture, Spec, params, rangedDamageMultiplier);
	GetCapturedAttributeMagnitude(RangedItemPowerFactorCapture, Spec, params, rangedItemPowerFactor);
	GetCapturedAttributeMagnitude(TakeRangeDamageAttributeCapture, Spec, params, targetRangeDamageMultiplier);

	return difficultyScaledDamage * rangedItemPowerFactor * rangedDamageMultiplier * targetRangeDamageMultiplier;
}

float URangedDamageModCalculation::PremultiplyAdd(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& params) const {
	auto addition = 0.f;
	GetCapturedAttributeMagnitude(RangedDamageIncreaseCapture, Spec, params, addition);
	return -addition;
}


UMeleeItemPowerOnlyModDamageCalculation::UMeleeItemPowerOnlyModDamageCalculation()
	: MeleeItemPowerFactorCapture(UMeleeAttributeSet::MeleeAttackItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	RelevantAttributesToCapture.Emplace(MeleeItemPowerFactorCapture);
}

float UMeleeItemPowerOnlyModDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const float difficultyScaledDamage = Super::CalculateBaseMagnitude_Implementation(Spec);
	float meleeItemPowerFactor = 1.f;
	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(MeleeItemPowerFactorCapture, Spec, params, meleeItemPowerFactor);

	return difficultyScaledDamage * meleeItemPowerFactor;
}


URangedItemPowerOnlyModDamageCalculation::URangedItemPowerOnlyModDamageCalculation()
	: RangedItemPowerFactorCapture(URangedAttributeSet::RangedAttackItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true) {
	RelevantAttributesToCapture.Emplace(RangedItemPowerFactorCapture);
}

float URangedItemPowerOnlyModDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const float difficultyScaledDamage = Super::CalculateBaseMagnitude_Implementation(Spec);
	float rangedItemPowerFactor = 1.f;
	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(RangedItemPowerFactorCapture, Spec, params, rangedItemPowerFactor);

	return difficultyScaledDamage * rangedItemPowerFactor;
}


float UItemPowerOnlyAsEffectLevelModDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec & Spec) const {
	return Super::CalculateBaseMagnitude_Implementation(Spec) * game::item::power::DamageIncreaseMultiplier(Spec.GetLevel());
}


UArmorItemPowerOnlyModDamageCalculation::UArmorItemPowerOnlyModDamageCalculation()
	: ArmorItemPowerFactorCapture(UHealthAttributeSet::ArmorAttackItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true) {
	RelevantAttributesToCapture.Emplace(ArmorItemPowerFactorCapture);
}

float UArmorItemPowerOnlyModDamageCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const float damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	float armorItemPowerFactor = 1.f;
	FAggregatorEvaluateParameters params;
	GetCapturedAttributeMagnitude(ArmorItemPowerFactorCapture, Spec, params, armorItemPowerFactor);

	return damage * armorItemPowerFactor;
}


UWorldDamageModCalculation::UWorldDamageModCalculation() {
	bAllowNonNetAuthorityDependencyRegistration = false;
}

float UWorldDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	const FDungeonsGameplayEffectContext* context = effects::GetDungeonsContextFromSpec(Spec);

	const game::DifficultyStats& difficultyStats = context->Game ? context->Game->settings().difficultyStats : game::CachedDifficultyStats::GetDefault();

	const float difficultyMultiplier = difficultyStats.GetMobDamageMultiplier();

	const float damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	return damage * difficultyMultiplier;
}

UReflectedDamageModCalculation::UReflectedDamageModCalculation()
	: TakeDamageAttributeCapture(UHealthAttributeSet::TakeDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(TakeDamageAttributeCapture);
}

float UReflectedDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float SourceTakeDamageMultiplier = 1.f;
	GetCapturedAttributeMagnitude(TakeDamageAttributeCapture, Spec, FAggregatorEvaluateParameters(), SourceTakeDamageMultiplier);

	const float damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	return damage / SourceTakeDamageMultiplier;
}

UReflectedRangeItemPowerDamageModCalculation::UReflectedRangeItemPowerDamageModCalculation()
	: TakeDamageAttributeCapture(UHealthAttributeSet::TakeDamageMultiplierAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
	, RangedItemPowerFactorCapture(URangedAttributeSet::RangedAttackItemPowerFactorAttribute(), EGameplayEffectAttributeCaptureSource::Source, true)
{
	bAllowNonNetAuthorityDependencyRegistration = false;
	RelevantAttributesToCapture.Emplace(TakeDamageAttributeCapture);
	RelevantAttributesToCapture.Emplace(RangedItemPowerFactorCapture);
}

float UReflectedRangeItemPowerDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	float SourceTakeDamageMultiplier = 1.f;
	GetCapturedAttributeMagnitude(TakeDamageAttributeCapture, Spec, FAggregatorEvaluateParameters(), SourceTakeDamageMultiplier);

	FAggregatorEvaluateParameters params;
	float rangedItemPowerFactor = 1.f;
	GetCapturedAttributeMagnitude(RangedItemPowerFactorCapture, Spec, params, rangedItemPowerFactor);

	const float damage = Super::CalculateBaseMagnitude_Implementation(Spec);
	return damage / SourceTakeDamageMultiplier * rangedItemPowerFactor;
}
