#pragma once

#include "game/levels.h"
#include "common_header.h"
#include "ItemTypeDefs.generated.h"

class ItemType;

UENUM(BlueprintType)
enum class ItemTag : uint8 {
	Unset,
	HealthPotion,
	Arrow,
	BurningArrow,
	FireworksArrow,
	TormentProjectile,
	ThunderingArrow,
	HeavyHarpoon,
	VoidArrow,
	PoisonArrow,
	MeleeWeapon,
	RangedWeapon,
	Armor,
	Currency,
	Item,
    Token,
	Food,
	Potion,
	UIFriendly,
	Last
};
 
UENUM(BlueprintType)
enum class ItemTagLevel : uint8 {
	Normal,
	Bright,
	Diamond,
	Last
};

// Sort by matching prio
UENUM(BlueprintType)
enum class ESlotType : uint8 {
	HealthPotion,
	Arrow,
	BurningArrow,
	FireworksArrow,
	TormentProjectile,
	HeavyHarpoon,
	ThunderingArrow,
	VoidArrow,
	PoisonArrow,
	TNT,
	Trident,
	Conduit,
	ActivePermanent,
	Consumable,
	MeleeWeapon,
	RangedWeapon,
	Armor,	
	Any,
	None,
	Last
};

namespace game { namespace item { 
	
namespace tag {
	TOptional<ItemTag> fromString(const FString& tag);
}
	
namespace type {
//Melee
extern const ItemType& HealthPotion;
extern const ItemType& Food1;
extern const ItemType& Food2;
extern const ItemType& Food3;
extern const ItemType& Food4;
extern const ItemType& Food5;
extern const ItemType& Food6;
extern const ItemType& Food7;
extern const ItemType& Food8;
extern const ItemType& Sword;
extern const ItemType& Sword_Unique1;
extern const ItemType& Sword_Unique2;
extern const ItemType& Sword_Spooky1;
extern const ItemType& Gauntlets;
extern const ItemType& Gauntlets_Unique1;
extern const ItemType& Gauntlets_Unique2;
extern const ItemType& Gauntlets_Unique3;
extern const ItemType& Battlestaff;
extern const ItemType& Battlestaff_Unique1;
extern const ItemType& Battlestaff_Unique2;
extern const ItemType& Sickles;
extern const ItemType& Sickles_Unique1;
extern const ItemType& Sickles_Unique2;
extern const ItemType& Claymore;
extern const ItemType& Claymore_Unique1;
extern const ItemType& Claymore_Unique2;
extern const ItemType& Claymore_Unique3;
extern const ItemType& Claymore_Winter1;
extern const ItemType& Spear;
extern const ItemType& Spear_Unique1;
extern const ItemType& Spear_Unique2;
extern const ItemType& Rapier;
extern const ItemType& Rapier_Unique1;
extern const ItemType& Rapier_Unique2;
extern const ItemType& Katana;
extern const ItemType& Katana_Unique1;
extern const ItemType& Katana_Unique2;
extern const ItemType& Glaive;
extern const ItemType& Glaive_Unique1;
extern const ItemType& Glaive_Unique2;
extern const ItemType& Glaive_Spooky2;
extern const ItemType& DoubleAxe;
extern const ItemType& DoubleAxe_Unique1;
extern const ItemType& DoubleAxe_Unique2;
extern const ItemType& Whip;
extern const ItemType& Whip_Unique1;
extern const ItemType& Boneclub;
extern const ItemType& Mace;
extern const ItemType& Mace_Unique1;
extern const ItemType& Mace_Unique2;
extern const ItemType& Chainsword;
extern const ItemType& Chainsword_Unique1;
extern const ItemType& Backstabber;
extern const ItemType& Backstabber_Unique1;
extern const ItemType& Cutlass;
extern const ItemType& Cutlass_Unique1;
extern const ItemType& Cutlass_Unique2;
extern const ItemType& Cutlass_Year1;
extern const ItemType& Daggers;
extern const ItemType& Daggers_Unique1;
extern const ItemType& Daggers_Unique2;
extern const ItemType& Daggers_Unique3;
extern const ItemType& Pickaxe;
extern const ItemType& Pickaxe_Unique1;
extern const ItemType& Pickaxe_Unique2;
extern const ItemType& Axe;
extern const ItemType& Axe_Unique1;
extern const ItemType& Axe_Unique2;
extern const ItemType& Hammer;
extern const ItemType& Hammer_Unique1;
extern const ItemType& Hammer_Unique2;
extern const ItemType& SoulScythe;
extern const ItemType& SoulScythe_Unique1;
extern const ItemType& SoulScythe_Unique2;
extern const ItemType& SoulScythe_Spooky2;
extern const ItemType& SoulKnife;
extern const ItemType& SoulKnife_Unique1;
extern const ItemType& SoulKnife_Unique2;
extern const ItemType& TempestKnife;
extern const ItemType& TempestKnife_Unique1;
extern const ItemType& TempestKnife_Unique2;
extern const ItemType& CoralBlade;
extern const ItemType& DifficultyToken;
extern const ItemType& MysteryToken;
extern const ItemType& ObsidianClaymore;
extern const ItemType& ObsidianClaymore_Unique1;
//Ranged
extern const ItemType& Bow;
extern const ItemType& Bow_Spooky1;
extern const ItemType& Bow_Unique1;
extern const ItemType& Bow_Unique2;
extern const ItemType& ShortBow;
extern const ItemType& ShortBow_Unique1;
extern const ItemType& ShortBow_Unique2;
extern const ItemType& ShortBow_Unique3;
extern const ItemType& Longbow;
extern const ItemType& Longbow_Unique1;
extern const ItemType& Longbow_Unique2;
extern const ItemType& TwistingVineBow;
extern const ItemType& PowerBow;
extern const ItemType& PowerBow_Unique1;
extern const ItemType& PowerBow_Unique2;
extern const ItemType& PowerBow_Spooky2;
extern const ItemType& HuntingBow;
extern const ItemType& HuntingBow_Unique1;
extern const ItemType& HuntingBow_Unique2;
extern const ItemType& HuntingBow_Unique3;
extern const ItemType& SoulBow;
extern const ItemType& SoulBow_Unique1;
extern const ItemType& SoulBow_Unique2;
extern const ItemType& CogCrossbow;
extern const ItemType& CogCrossbow_Unique1;
extern const ItemType& SoulBow_Winter1;
extern const ItemType& Crossbow;
extern const ItemType& Crossbow_Unique1;
extern const ItemType& Crossbow_Unique2;
extern const ItemType& SpiderCrossbow;
extern const ItemType& RapidCrossbow;
extern const ItemType& RapidCrossbow_Unique1;
extern const ItemType& RapidCrossbow_Unique2;
extern const ItemType& ScatterCrossbow;
extern const ItemType& ScatterCrossbow_Unique1;
extern const ItemType& ScatterCrossbow_Unique2;
extern const ItemType& BurstCrossbow;
extern const ItemType& BurstCrossbow_Unique1;
extern const ItemType& BurstCrossbow_Unique2;
extern const ItemType& DualCrossbow;
extern const ItemType& DualCrossbow_Unique1;
extern const ItemType& DualCrossbow_Unique2;
extern const ItemType& SoulCrossbow;
extern const ItemType& SoulCrossbow_Unique1;
extern const ItemType& SoulCrossbow_Unique2;
extern const ItemType& ExplodingCrossbow;
extern const ItemType& ExplodingCrossbow_Unique1;
extern const ItemType& ExplodingCrossbow_Unique2;
extern const ItemType& ShadowCrossbow;
extern const ItemType& ShadowCrossbow_Unique1;
extern const ItemType& SlowBow;
extern const ItemType& SlowBow_Unique1;
extern const ItemType& SlowBow_Spooky2;
extern const ItemType& WindBow;
extern const ItemType& WindBow_Unique1;
extern const ItemType& WindBow_Unique2;
extern const ItemType& VoidBow;

// Armor
extern const ItemType& ArchersStrappings;
extern const ItemType& ArchersStrappings_Unique1;
extern const ItemType& CowardsArmor;
extern const ItemType& CowardsArmor_Unique1;
extern const ItemType& ReinforcedMail;
extern const ItemType& ReinforcedMail_Unique1;
extern const ItemType& SproutArmor;
extern const ItemType& SproutArmor_Unique1;
extern const ItemType& PiglinArmor;
extern const ItemType& PiglinArmor_Unique1;
extern const ItemType& ScaleMail;
extern const ItemType& ScaleMail_Unique1;
extern const ItemType& GhostArmor;
extern const ItemType& GhostArmor_Unique1;
extern const ItemType& EvocationRobe;
extern const ItemType& EvocationRobe_Unique1;
extern const ItemType& EvocationRobe_Unique2;
extern const ItemType& AssassinArmor;
extern const ItemType& AssassinArmor_Unique1;
extern const ItemType& BattleRobe;
extern const ItemType& BattleRobe_Unique1;
extern const ItemType& DarkArmor;
extern const ItemType& DarkArmor_Unique1;
extern const ItemType& WolfArmor;
extern const ItemType& WolfArmor_Unique1;
extern const ItemType& WolfArmor_Unique2;
extern const ItemType& WolfArmor_Winter1;
extern const ItemType& ChampionsArmor;
extern const ItemType& ChampionsArmor_Unique1;
extern const ItemType& MercenaryArmor;
extern const ItemType& MercenaryArmor_Unique1;
extern const ItemType& MercenaryArmor_Spooky1;
extern const ItemType& SpelunkersArmor;
extern const ItemType& SpelunkersArmor_Unique1;
extern const ItemType& SoulRobe;
extern const ItemType& SoulRobe_Unique1;
extern const ItemType& GrimArmor;
extern const ItemType& GrimArmor_Unique1;
extern const ItemType& GrimArmor_Spooky2;
extern const ItemType& PhantomArmor;
extern const ItemType& PhantomArmor_Unique1;
extern const ItemType& FullPlateArmor;
extern const ItemType& FullPlateArmor_Unique1;
extern const ItemType& FullPlateArmor_Spooky2;
extern const ItemType& MysteryArmor;
extern const ItemType& MysteryArmor_Unique1;
extern const ItemType& OcelotArmor;
extern const ItemType& OcelotArmor_Unique1;
extern const ItemType& SnowArmor;
extern const ItemType& SnowArmor_Unique1;
extern const ItemType& ClimbingGear;
extern const ItemType& ClimbingGear_Unique1;
extern const ItemType& EmeraldArmor;
extern const ItemType& EmeraldArmor_Unique1;
extern const ItemType& SquidArmor;
extern const ItemType& SquidArmor_Unique1;
extern const ItemType& TurtleArmor;
extern const ItemType& TurtleArmor_Unique1;
extern const ItemType& ShulkerArmor;
extern const ItemType& ShulkerArmor_Unique1;
extern const ItemType& BardsGarb;
extern const ItemType& BardsGarb_Unique1;
extern const ItemType& Enderobes;
extern const ItemType& Enderobes_Unique1;
//Items
extern const ItemType& Arrow;
extern const ItemType& BurningArrow;
extern const ItemType& FireworksArrow;
extern const ItemType& FireworksArrowItem;
extern const ItemType& TormentProjectile;
extern const ItemType& ThunderingArrow;
extern const ItemType& Harvester;
extern const ItemType& Harvester_Unique1;
extern const ItemType& TNTBox;
extern const ItemType& Conduit;
extern const ItemType& GongOfWeakening;
extern const ItemType& LightFeather;
extern const ItemType& WindHorn;
extern const ItemType& BootsOfSwiftness;
extern const ItemType& GhostCloak;
extern const ItemType& FishingRod;
extern const ItemType& EnderPearl;
extern const ItemType& SwiftnessPotion;
extern const ItemType& StrengthPotion;
extern const ItemType& DefensePotion;
extern const ItemType& IcePotion;
extern const ItemType& DenseBrewPotion;
extern const ItemType& WaterBreathingPotion;
extern const ItemType& SplashSlowingPotion;
extern const ItemType& BurningBrewPotion;
extern const ItemType& DeathCapMushroom;
extern const ItemType& ShockPowder;
extern const ItemType& IronHideAmulet;
extern const ItemType& BurningOilVial;
extern const ItemType& PlentifulQuiver;
extern const ItemType& RecyclerQuiver;
extern const ItemType& BackstabbersBrew;
extern const ItemType& Emerald;
extern const ItemType& Gold;
extern const ItemType& TastyBone;
extern const ItemType& WonderfulWheat;
extern const ItemType& GolemKit;
extern const ItemType& LoveMedallion;
extern const ItemType& CorruptedBeacon;
extern const ItemType& CorruptedBeacon_Spooky1;
extern const ItemType& DiamondDust;
extern const ItemType& LightningRod;
extern const ItemType& SoulHealer;
extern const ItemType& TotemOfShielding;
extern const ItemType& TotemOfShielding_Unique1;
extern const ItemType& TotemOfSoulProtection;
extern const ItemType& TotemOfRegeneration;
extern const ItemType& FlamingQuiver;
extern const ItemType& CorruptedSeeds;
extern const ItemType& IceWand;
extern const ItemType& NecromancersStaff;
extern const ItemType& BeeNest;
extern const ItemType& RainbowGrass;
extern const ItemType& SoulLatern;
extern const ItemType& NetherWartSporeGrenade;
extern const ItemType& FireworkBomb;
extern const ItemType& MobMasher;
extern const ItemType& SpinWheel;
extern const ItemType& SatchelOfTheElements;
extern const ItemType& UpdraftTome;
extern const ItemType& EnchantersTome;
extern const ItemType& ChargedRedstoneMines;
extern const ItemType& Trident;
extern const ItemType& Conduit;
extern const ItemType& EyeOfEnder;
extern const ItemType& MysteryBoxAny;
extern const ItemType& MysteryBoxArtifact;
extern const ItemType& MysteryBoxGear;
extern const ItemType& MysteryBoxMelee;
extern const ItemType& MysteryBoxRanged;
extern const ItemType& MysteryBoxArmor;
extern const ItemType& GiftBox;
extern const ItemType& GuardianEye;
extern const ItemType& SatchelOfNourishment;
extern const ItemType& SatchelOfNeed;
extern const ItemType& HeavyHarpoon;
extern const ItemType& HeavyDutyHarpoonQuiver;
extern const ItemType& Trident;
extern const ItemType& ShadowSplinter;
extern const ItemType& Elytra;
extern const ItemType& LostEvocation;
extern const ItemType& VoidQuiver;
extern const ItemType& VoidArrow;
extern const ItemType& PoisonArrow;
}}}
