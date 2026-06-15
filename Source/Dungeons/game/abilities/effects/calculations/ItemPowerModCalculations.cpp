#include "Dungeons.h"
#include "ItemPowerModCalculations.h"
#include "game/item/power/ItemPowerEffectDefs.h"
#include "game/item/power/ItemPowerStats.h"


float UItemPowerMeleeDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return game::item::power::DamageIncreaseMultiplier(Spec.GetLevel());
}

float UItemPowerMeleeHealingModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return game::item::power::HealingIncreaseMultiplier(Spec.GetLevel());
}

float UItemPowerRangedDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return game::item::power::DamageIncreaseMultiplier(Spec.GetLevel());
}

float UItemPowerRangedHealingModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return game::item::power::HealingIncreaseMultiplier(Spec.GetLevel());
}

float UItemPowerHealthModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return game::item::power::MaxHealthMultiplier(Spec.GetLevel());
}

float UItemPowerArmorDamageModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return game::item::power::DamageIncreaseMultiplier(Spec.GetLevel());
}

float UItemPowerArmorHealingModCalculation::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const {
	return game::item::power::HealingIncreaseMultiplier(Spec.GetLevel());
}
