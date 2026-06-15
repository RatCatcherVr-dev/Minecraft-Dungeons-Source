#pragma once

#include "ArmorPropertyEnumTypes.Generated.h"


UENUM(BlueprintType)
enum class EArmorPropertyID : uint8 {
	Unset,
	SoulGatheringBoost,
	SuperbDamageAbsorption,
	MissChance,
	TeleportChance,
	ItemDamageBoost,
	ItemCooldownDecrease,
	AllyDamageBoost,
	IncreasedArrowBundleSize,
	MeleeDamageBoost,
	MeleeAttackSpeedBoost,
	RangedDamageBoost,
	LifeStealAura,
	MoveSpeedAura,
	PetBat,
	AreaHeal,
	MoveSpeedReduction,
	IncreasedMobTargeting,
	PotionCooldownDecrease,
	DodgeCooldownIncrease,
	DodgeSpeedIncrease,
	DodgeInvulnerability,
	DamageAbsorption,
	SlowResistance,
	DodgeGhostForm,
	Beekeeper,
	DodgeRoot,
	Heavyweight,
	EnvironmentalProtection,
	EmeraldShield,
	ItemCooldownReset,
	SquidRollQuick,
	SquidRollLimited,
	HealingAura,
	FallResistance,
	SquidRoll,
	Resonant,
	ImmunityBoost,
	InstantTransmission,
	Last
};
ENUM_NAME(EArmorPropertyID);

UENUM(BlueprintType)
enum class EArmorPropertyClassification : uint8 {
	Positive,
	Negative
};
ENUM_NAME(EArmorPropertyClassification);

UENUM(BlueprintType)
enum class EArmorPropertyTag : uint8 {
	SoulGatheringBoost,
};
ENUM_NAME(EArmorPropertyTag);


