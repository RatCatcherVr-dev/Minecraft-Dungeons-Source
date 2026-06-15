#pragma once
#include "ArmorPropertyType.h"

namespace game { namespace armorproperties { namespace type {

extern const ArmorPropertyType& SoulGatheringBoost;
extern const ArmorPropertyType& SuperbDamageAbsorption;
extern const ArmorPropertyType& MissChance;
extern const ArmorPropertyType& TeleportChance;
extern const ArmorPropertyType& ItemDamageBoost;
extern const ArmorPropertyType& ItemCooldownDecrease;
extern const ArmorPropertyType& AllyDamageBoost;
extern const ArmorPropertyType& IncreasedArrowBundleSize;
extern const ArmorPropertyType& MeleeDamageBoost;
extern const ArmorPropertyType& MeleeAttackSpeedBoost;
extern const ArmorPropertyType& RangedDamageBoost;
extern const ArmorPropertyType& LifeStealAura;
extern const ArmorPropertyType& MoveSpeedAura;
extern const ArmorPropertyType& PetBat;
extern const ArmorPropertyType& AreaHeal;
extern const ArmorPropertyType& MoveSpeedReduction;
extern const ArmorPropertyType& IncreasedMobTargeting;
extern const ArmorPropertyType& PotionCooldownDecrease;
extern const ArmorPropertyType& DodgeCooldownIncrease;
extern const ArmorPropertyType& SlowResistance;
extern const ArmorPropertyType& DodgeGhostform;
extern const ArmorPropertyType& Beekeeper;
extern const ArmorPropertyType& DodgeRoot;
extern const ArmorPropertyType& EmeraldShield;
extern const ArmorPropertyType& ItemCooldownReset;
extern const ArmorPropertyType& EmeraldShield;
extern const ArmorPropertyType& ImmunityBoost;
extern const ArmorPropertyType& Resonant;
extern const ArmorPropertyType& InstantTransmission;

const ArmorPropertyType& getArmorPropertyType(EArmorPropertyID id);

const TArray<ArmorPropertyType> getArmorPropertiesOfClassification(EArmorPropertyClassification classification);
const TArray<ArmorPropertyType> getArmorProperties();

void PreloadArmorPropertyTypes();

}}}
