#pragma once

#include "EnchantmentType.h"
#include "CoreMinimal.h"

namespace game { namespace enchantment {

	constexpr int RowSize = 3;
	constexpr int MaxNumEnchantments = RowSize * 3;
	constexpr int MaxPointsPerEnchantment = 3;
	constexpr int RequiredPointsPerRow = 1;

	static int UnlockedRows = 3;

	namespace type {

	EEnchantmentCategory EnchantmentCategoryFromSlotType(ESlotType type);

	EEnchantmentCategory EnchantmentCategoryFromItemTag(ItemTag type);

	// Melee
	extern const EnchantmentType& Unset;
	extern const EnchantmentType& Sharpness;
	extern const EnchantmentType& Knockback;
	extern const EnchantmentType& Looting;
	extern const EnchantmentType& Prospector;
	extern const EnchantmentType& FireAspect;
	extern const EnchantmentType& Rampaging;
	extern const EnchantmentType& Exploding;
	extern const EnchantmentType& CriticalHit;
	extern const EnchantmentType& Freezing;
	extern const EnchantmentType& PoisonedMelee;
	extern const EnchantmentType& Leeching;
	extern const EnchantmentType& Aiding;
	extern const EnchantmentType& GravityMelee;
	extern const EnchantmentType& EnigmaResonatorMelee;
	extern const EnchantmentType& AnimaConduitMelee;
	extern const EnchantmentType& Stunning;
	extern const EnchantmentType& CaveSpiderPoisonEnchantment;
	extern const EnchantmentType& Swirling;
	extern const EnchantmentType& Smiting;
	extern const EnchantmentType& Committed;
	extern const EnchantmentType& SoulSiphon;
	extern const EnchantmentType& RadianceMelee;
	extern const EnchantmentType& Chains;
	extern const EnchantmentType& Thundering;
	extern const EnchantmentType& Echo;
	extern const EnchantmentType& Shockwave;
	extern const EnchantmentType& Weakening;
	extern const EnchantmentType& BusyBee;
	extern const EnchantmentType& DynamoMelee;
	extern const EnchantmentType& BaneOfIllagers;
	extern const EnchantmentType& GuardingStrike;
	extern const EnchantmentType& PotionThirstMelee;
	extern const EnchantmentType& WitherEnchantmentMelee;
	extern const EnchantmentType& VoidTouchedMelee;
	extern const EnchantmentType& SharedPain;
	extern const EnchantmentType& BlindMelee;
	extern const EnchantmentType& Backstabber;

	//Ranged
	extern const EnchantmentType& TempoTheft;
	extern const EnchantmentType& AlacrityAdjustment;
	extern const EnchantmentType& BowsBoon;
	extern const EnchantmentType& Ricochet;
	extern const EnchantmentType& Power;
	extern const EnchantmentType& Punch;
	extern const EnchantmentType& Infinity;
	extern const EnchantmentType& MultiShot;
	extern const EnchantmentType& Piercing;
	extern const EnchantmentType& ChainReaction;
	extern const EnchantmentType& GravityRanged;
	extern const EnchantmentType& EnigmaResonatorRanged;
	extern const EnchantmentType& AnimaConduitRanged;
	extern const EnchantmentType& PoisonedRanged;
	extern const EnchantmentType& RapidFire;
	extern const EnchantmentType& Supercharge;
	extern const EnchantmentType& BonusShot;
	extern const EnchantmentType& Unchanting;
	extern const EnchantmentType& FuseShot;
	extern const EnchantmentType& RadianceRanged;
	extern const EnchantmentType& Accelerating;
	extern const EnchantmentType& Growing;
	extern const EnchantmentType& WildRage;
	extern const EnchantmentType& DynamoRanged;
	extern const EnchantmentType& BurstBowstring;
	extern const EnchantmentType& ChargingAcceleration;
	extern const EnchantmentType& CogCrossbowEnchantment;
	extern const EnchantmentType& TwistingVine;
	extern const EnchantmentType& TwistingUnique;
	extern const EnchantmentType& PotionThirstRanged;
	extern const EnchantmentType& CooldownShot;
	extern const EnchantmentType& ArtifactCharge;
	extern const EnchantmentType& WitherEnchantmentRanged;
	extern const EnchantmentType& ShockWeb;
	extern const EnchantmentType& VoidTouchedRanged;
	extern const EnchantmentType& ShadowShot;
	extern const EnchantmentType& DippingPoison;

	// Armor
	extern const EnchantmentType& Protection;
	extern const EnchantmentType& Celerity;
	extern const EnchantmentType& FinalShout;
	extern const EnchantmentType& Deflecting;
	extern const EnchantmentType& Regeneration;
	extern const EnchantmentType& Thorns;
	extern const EnchantmentType& AncientGuardianThorns;
	extern const EnchantmentType& Altruistic;
	extern const EnchantmentType& Shielding;
	extern const EnchantmentType& Barrier;
	extern const EnchantmentType& Recycler;
	extern const EnchantmentType& Chilling;
	extern const EnchantmentType& Cowardice;
	extern const EnchantmentType& Electrified;
	extern const EnchantmentType& Burning;
	extern const EnchantmentType& Snowing;
	extern const EnchantmentType& GravityPulse;
	extern const EnchantmentType& FireTrail;
	extern const EnchantmentType& Frenzied;
	extern const EnchantmentType& SwiftFooted;
	extern const EnchantmentType& SpiritSpeed;
	extern const EnchantmentType& PotionFortification;
	extern const EnchantmentType& FoodReserves;
	extern const EnchantmentType& IngredientPockets;
	extern const EnchantmentType& DoubleDamage;
	extern const EnchantmentType& FastAttack;
	extern const EnchantmentType& Quick;
	extern const EnchantmentType& HealthSynergy;
	extern const EnchantmentType& SpeedSynergy;
	extern const EnchantmentType& Explorer;
	extern const EnchantmentType& SurpriseGift;
	extern const EnchantmentType& VesselTrail;
	extern const EnchantmentType& TumbleBee;
	extern const EnchantmentType& BagOfSouls;
	extern const EnchantmentType& Acrobat;
	extern const EnchantmentType& RollCharge;
	extern const EnchantmentType& ResurrectionSurge;
	extern const EnchantmentType& Huge;
	extern const EnchantmentType& ResurrectSurroundingMobs;
	extern const EnchantmentType& Invisible;
	extern const EnchantmentType& Venomancer;
	extern const EnchantmentType& Pyromancer;
	extern const EnchantmentType& Animancer;
	extern const EnchantmentType& Electromancer;
	extern const EnchantmentType& Flee;
	extern const EnchantmentType& Reckless;
	extern const EnchantmentType& CurrentResistance;
	extern const EnchantmentType& CurrentImmunity;
	extern const EnchantmentType& ThriveUnderPressure;
	extern const EnchantmentType& UnderwaterImmunity;

	const EnchantmentType& getEnchantmentType(EEnchantmentTypeID typeID);
	const TArray<EnchantmentType>& getAvailableEnchantments();
}}}
