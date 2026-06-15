#include "Dungeons.h"
#include "ItemPowerGameplayEffects.h"
#include "calculations/ItemPowerModCalculations.h"
#include "calculations/HealingModCalculations.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "game/abilities/attributes/RangedAttributeSet.h"
#include "game/abilities/attributes/HealthAttributeSet.h"

UMeleeDamageItemPowerGameplayEffect::UMeleeDamageItemPowerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FCustomCalculationBasedFloat itemPowerScaling;
	itemPowerScaling.CalculationClassMagnitude = UItemPowerMeleeDamageModCalculation::StaticClass();

	FGameplayModifierInfo itemPowerModifier;
	itemPowerModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	itemPowerModifier.ModifierMagnitude = itemPowerScaling;
	itemPowerModifier.Attribute = UMeleeAttributeSet::MeleeAttackItemPowerFactorAttribute();
	Modifiers.Add(itemPowerModifier);
}

UMeleeHealingItemPowerGameplayEffect::UMeleeHealingItemPowerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FCustomCalculationBasedFloat itemPowerScaling;
	itemPowerScaling.CalculationClassMagnitude = UItemPowerMeleeHealingModCalculation::StaticClass();

	FGameplayModifierInfo itemPowerModifier;
	itemPowerModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	itemPowerModifier.ModifierMagnitude = itemPowerScaling;
	itemPowerModifier.Attribute = UMeleeAttributeSet::MeleeHealItemPowerFactorAttribute();
	Modifiers.Add(itemPowerModifier);
}

URangedDamageItemPowerGameplayEffect::URangedDamageItemPowerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FCustomCalculationBasedFloat itemPowerScaling;
	itemPowerScaling.CalculationClassMagnitude = UItemPowerRangedDamageModCalculation::StaticClass();

	FGameplayModifierInfo ItemPowerModifier;
	ItemPowerModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	ItemPowerModifier.ModifierMagnitude = itemPowerScaling;
	ItemPowerModifier.Attribute = URangedAttributeSet::RangedAttackItemPowerFactorAttribute();
	Modifiers.Add(ItemPowerModifier);
}

URangedHealingItemPowerGameplayEffect::URangedHealingItemPowerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FCustomCalculationBasedFloat itemPowerScaling;
	itemPowerScaling.CalculationClassMagnitude = UItemPowerRangedHealingModCalculation::StaticClass();

	FGameplayModifierInfo ItemPowerModifier;
	ItemPowerModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	ItemPowerModifier.ModifierMagnitude = itemPowerScaling;
	ItemPowerModifier.Attribute = URangedAttributeSet::RangedHealItemPowerFactorAttribute();
	Modifiers.Add(ItemPowerModifier);
}

UMaxHealthItemPowerGameplayEffect::UMaxHealthItemPowerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FCustomCalculationBasedFloat maxHealthScaling;
	maxHealthScaling.CalculationClassMagnitude = UItemPowerHealthModCalculation::StaticClass();

	FGameplayModifierInfo maxHealthModifier;
	maxHealthModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	maxHealthModifier.ModifierMagnitude = maxHealthScaling;
	maxHealthModifier.Attribute = UHealthAttributeSet::MaxHealthAttribute();
	Modifiers.Add(maxHealthModifier);
}


UArmorDamageItemPowerGameplayEffect::UArmorDamageItemPowerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FCustomCalculationBasedFloat attackPowerScaling;
	attackPowerScaling.CalculationClassMagnitude = UItemPowerArmorDamageModCalculation::StaticClass();
	
	FGameplayModifierInfo attackPowerModifier;
	attackPowerModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	attackPowerModifier.ModifierMagnitude = attackPowerScaling;
	attackPowerModifier.Attribute = UHealthAttributeSet::ArmorAttackItemPowerFactorAttribute();
	Modifiers.Add(attackPowerModifier);
}

UArmorHealingItemPowerGameplayEffect::UArmorHealingItemPowerGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FCustomCalculationBasedFloat itemPowerScaling;
	itemPowerScaling.CalculationClassMagnitude = UItemPowerArmorHealingModCalculation::StaticClass();

	FGameplayModifierInfo ItemPowerModifier;
	ItemPowerModifier.ModifierOp = EGameplayModOp::Multiplicitive;
	ItemPowerModifier.ModifierMagnitude = itemPowerScaling;
	ItemPowerModifier.Attribute = UHealthAttributeSet::ArmorHealingItemPowerFactorAttribute();
	Modifiers.Add(ItemPowerModifier);
}
