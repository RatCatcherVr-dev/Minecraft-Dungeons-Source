#include "Dungeons.h"
#include "CommonTypes.h"
#include "MutableMobTypeDef.h"
#include "util/CollectionUtils.h"
#include "util/Algo.h"
#include "MobTypeDefs.h"

namespace mob { namespace type {

auto mobTypeDefs = TMap<EntityType, Unique<MutableMobTypeDef>>();

MutableMobTypeDef& create(EntityType mobType) {
	check(!mobTypeDefs.Contains(mobType));
	mobTypeDefs.Add(mobType, make_unique<MutableMobTypeDef>(mobType));
	return *mobTypeDefs[mobType];
}

#define LOCTEXT_NAMESPACE "MobType"

//
// Mob Types collection
//

const MobTypeDef& Invalid = create(EntityType::Undefined)
	.disabled()
	;


const MobTypeDef& Chicken = create(EntityType::Chicken)
	.name(LOCTEXT("mob_chicken", "Chicken"))	
;
const MobTypeDef& Cow = create(EntityType::Cow)
	.name(LOCTEXT("mob_Cow", "Cow"))
;
const MobTypeDef& Pig = create(EntityType::Pig)
	.name(LOCTEXT("mob_Pig", "Pig"))
;
const MobTypeDef& Sheep = create(EntityType::Sheep)
	.name(LOCTEXT("mob_Sheep", "Sheep"))
;
const MobTypeDef& SheepFireRed = create(EntityType::SheepFireRed)
	.name(LOCTEXT("mob_SheepFireRed", "Fiery Sheep"))
;
const MobTypeDef& SheepPoisonGreen = create(EntityType::SheepPoisonGreen)
	.name(LOCTEXT("mob_SheepPoisonGreen", "Poisoned Sheep"))
;
const MobTypeDef& SheepSpeedBlue = create(EntityType::SheepSpeedBlue)
	.name(LOCTEXT("mob_SheepSpeedBlue", "Speedy Sheep"))
;
const MobTypeDef& Wolf = create(EntityType::Wolf)
	.name(LOCTEXT("mob_Wolf", "Wolf"))
;
const MobTypeDef& Villager = create(EntityType::Villager)
	.name(LOCTEXT("mob_Villager", "Villager"))
;
const MobTypeDef& Mooshroom = create(EntityType::Mooshroom)
	.name(LOCTEXT("mob_Mooshroom", "Mooshroom"))
;
const MobTypeDef& MooshroomAncient = create(EntityType::MooshroomAncient)
	.iconRowName(EntityType::Mooshroom)
	.name(LOCTEXT("mob_MooshroomAncient", "?????"))
;
const MobTypeDef& Squid = create(EntityType::Squid)
	.name(LOCTEXT("mob_Squid", "Squid"))
;
const MobTypeDef& GlowSquid = create(EntityType::GlowSquid)
.name(LOCTEXT("mob_GlowSquid", "GlowSquid"))
;
const MobTypeDef& Rabbit = create(EntityType::Rabbit)
	.name(LOCTEXT("mob_Rabbit", "Rabbit"))
;
const MobTypeDef& Bat = create(EntityType::Bat)
	.name(LOCTEXT("mob_Bat", "Bat"))
;
const MobTypeDef& IronGolem = create(EntityType::IronGolem)
	.name(LOCTEXT("mob_IronGolem", "Iron Golem"))
;
const MobTypeDef& SnowGolem = create(EntityType::SnowGolem)
	.name(LOCTEXT("mob_SnowGolem", "Snow Golem"))
;
const MobTypeDef& Horse = create(EntityType::Horse)
	.name(LOCTEXT("mob_Horse", "Horse"))
;
const MobTypeDef& BabyGhast = create(EntityType::BabyGhast)
.name(LOCTEXT("mob_BabyGhast", "BabyGhast"))
;


const MobTypeDef& ZombifiedBabyPig = create(EntityType::ZombifiedBabyPig)
.name(LOCTEXT("mob_ZombifiedBabyPig", "ZombifiedBabyPig"))


;
const MobTypeDef& PolarBear = create(EntityType::PolarBear)
	.name(LOCTEXT("mob_PolarBear", "Polar Bear"))
;
const MobTypeDef& BabyGoat = create(EntityType::BabyGoat)
.name(LOCTEXT("mob_BabyGoat", "Baby Goat"))
;
const MobTypeDef& Goat = create(EntityType::Goat)
.name(LOCTEXT("mob_Goat", "Goat"))
;
const MobTypeDef& Windbeard = create(EntityType::GoatAncient)
	.iconRowName(EntityType::Goat)
	.name(LOCTEXT("mob_Windbeard", "Windbeard"))
;
const MobTypeDef& Ravager = create(EntityType::Ravager)
.name(LOCTEXT("mob_Ravager", "Ravager"))
;
const MobTypeDef& Mountaineer = create(EntityType::Mountaineer)
.name(LOCTEXT("mob_Mountaineer", "Mountaineer"))
;
const MobTypeDef& MountaineerVariant0 = create(EntityType::MountaineerVariant0)
.iconRowName(EntityType::Mountaineer)
.name(LOCTEXT("mob_Mountaineer", "Mountaineer"))
;
const MobTypeDef& MountaineerVariant1 = create(EntityType::MountaineerVariant1)
.iconRowName(EntityType::Mountaineer)
.name(LOCTEXT("mob_Mountaineer", "Mountaineer"))
;
const MobTypeDef& MountaineerVariant2 = create(EntityType::MountaineerVariant2)
.iconRowName(EntityType::Mountaineer)
.name(LOCTEXT("mob_Mountaineer", "Mountaineer"))
;
const MobTypeDef& WindCaller = create(EntityType::WindCaller)
.name(LOCTEXT("mob_WindCaller", "Wind Caller"))
;
const MobTypeDef& SquallGolem = create(EntityType::SquallGolem)
.name(LOCTEXT("mob_SquallGolem", "Squall Golem"))
;
const MobTypeDef& TempestGolem = create(EntityType::TempestGolem)
.name(LOCTEXT("mob_TempestGolem", "Tempest Golem"))
;
const MobTypeDef& WoolyCow = create(EntityType::WoolyCow)
.name(LOCTEXT("mob_WoolyCow", "Wooly Cow"))
;

//These are temporary and only used for mob spawner items
const MobTypeDef& Donkey = create(EntityType::Donkey)
	.name(LOCTEXT("mob_Donkey", "Donkey"))
;
const MobTypeDef& Mule = create(EntityType::Mule)
	.name(LOCTEXT("mob_Mule", "Mule"))
;
const MobTypeDef& SkeletonHorse = create(EntityType::SkeletonHorse)
	.name(LOCTEXT("mob_SkeletonHorse", "Skeleton Horse"))
;
const MobTypeDef& ZombieHorse = create(EntityType::ZombieHorse)
	.name(LOCTEXT("mob_ZombieHorse", "Zombie Horse"))
;

const MobTypeDef& RedstoneGolem = create(EntityType::RedstoneGolem)
	.name(LOCTEXT("mob_RedstoneGolem", "Redstone Golem"))
;
const MobTypeDef& UnbreakableOne = create(EntityType::RedstoneGolemAncient)
	.iconRowName(EntityType::RedstoneGolem)
	.name(LOCTEXT("mob_UnbreakableOne", "Unbreakable One"))
;
const MobTypeDef& Enchanter = create(EntityType::Enchanter)
	.name(LOCTEXT("mob_Enchanter", "Enchanter"))
;
const MobTypeDef& FirstEnchanter = create(EntityType::EnchanterAncient)
	.iconRowName(EntityType::Enchanter)
	.name(LOCTEXT("mob_FirstEnchanter", "First Enchanter"))
;
const MobTypeDef& Vindicator = create(EntityType::Vindicator)
	.name(LOCTEXT("mob_Vindicator", "Vindicator"))
;
const MobTypeDef& VindicatorVariant0 = create(EntityType::VindicatorVariant0)
	.iconRowName(EntityType::Vindicator)
	.name(LOCTEXT("mob_Vindicator", "Vindicator"))
;
const MobTypeDef& VindicatorVariant1 = create(EntityType::VindicatorVariant1)
	.iconRowName(EntityType::Vindicator)
	.name(LOCTEXT("mob_Vindicator", "Vindicator"))
;
const MobTypeDef& VindicatorVariant2 = create(EntityType::VindicatorVariant2)
	.iconRowName(EntityType::Vindicator)
	.name(LOCTEXT("mob_Vindicator", "Vindicator"))
;

// note that type Monster == (Monster | PathfinderMob | Mob)
const MobTypeDef& BabyZombie = create(EntityType::BabyZombie)
	.name(LOCTEXT("mob_BabyZombie", "Baby Zombie"))
;
const MobTypeDef& BabyDrowned = create(EntityType::BabyDrowned)
.name(LOCTEXT("mob_BabyDrowned", "Baby Drowned"))
;
const MobTypeDef& TheTinyScourge = create(EntityType::BabyZombieAncient)
	.iconRowName(EntityType::BabyZombie)
	.name(LOCTEXT("mob_TheTinyScourge", "The Tiny Scourge"))
;
const MobTypeDef& Zombie = create(EntityType::Zombie)
	.name(LOCTEXT("mob_Zombie", "Zombie"))
;
const MobTypeDef& ZombieVariant0 = create(EntityType::ZombieVariant0)
	.iconRowName(EntityType::Zombie)
	.name(LOCTEXT("mob_Zombie", "Zombie"))
;
const MobTypeDef& ZombieVariant1 = create(EntityType::ZombieVariant1)
	.iconRowName(EntityType::Zombie)
	.name(LOCTEXT("mob_Zombie", "Zombie"))
;
const MobTypeDef& ZombieVariant2 = create(EntityType::ZombieVariant2)
	.iconRowName(EntityType::Zombie)
	.name(LOCTEXT("mob_Zombie", "Zombie"))
;
const MobTypeDef& GrimGuardian = create(EntityType::ZombieAncient)
	.iconRowName(EntityType::Zombie)
	.name(LOCTEXT("mob_GrimGuardian", "Grim Guardian"))
;
const MobTypeDef& Creeper = create(EntityType::Creeper)
	.name(LOCTEXT("mob_Creeper", "Creeper"))
;

const MobTypeDef& ChargedCreeper = create(EntityType::ChargedCreeper)
	.iconRowName(EntityType::Creeper)
.name(LOCTEXT("mob_ChargedCreeper", "Charged Creeper"))
;

const MobTypeDef& Skeleton = create(EntityType::Skeleton)
	.name(LOCTEXT("mob_Skeleton", "Skeleton"))
;
const MobTypeDef& SkeletonVariant0 = create(EntityType::SkeletonVariant0)
	.iconRowName(EntityType::Skeleton)
	.name(LOCTEXT("mob_Skeleton", "Skeleton"))
;
const MobTypeDef& SkeletonVariant1 = create(EntityType::SkeletonVariant1)
	.iconRowName(EntityType::Skeleton)
	.name(LOCTEXT("mob_Skeleton", "Skeleton"))
;
const MobTypeDef& SkeletonVariant2 = create(EntityType::SkeletonVariant2)
	.iconRowName(EntityType::Skeleton)
	.name(LOCTEXT("mob_Skeleton", "Skeleton"))
;
const MobTypeDef& Barrage = create(EntityType::SkeletonAncient)
	.iconRowName(EntityType::Skeleton)
	.name(LOCTEXT("mob_Barrage", "Barrage"))
;
const MobTypeDef& Spider = create(EntityType::Spider)
	.name(LOCTEXT("mob_Spider", "Spider"))
;
const MobTypeDef& AbominableWeaver = create(EntityType::SpiderAncient)
	.iconRowName(EntityType::Spider)
	.name(LOCTEXT("mob_AbominableWeaver", "Abominable Weaver"))
;
const MobTypeDef& Silverfish = create(EntityType::Silverfish)
	.iconRowName(EntityType::Silverfish)
	.name(LOCTEXT("mob_Silverfish", "Silverfish"))
;
const MobTypeDef& SilverfishSmart = create(EntityType::SilverfishSmart)
	.name(LOCTEXT("mob_SilverfishSmart", "Silverfish"))
;
const MobTypeDef& ScuttlingTorment = create(EntityType::SilverfishAncient)
	.name(LOCTEXT("mob_ ScuttlingTorment", " Scuttling Torment"))
;
const MobTypeDef& CaveSpider = create(EntityType::CaveSpider)
	.name(LOCTEXT("mob_CaveSpider", "Cave Spider"))
;
const MobTypeDef& Ghast = create(EntityType::Ghast)
	.name(LOCTEXT("mob_Ghast", "Ghast"))
;
const MobTypeDef& MagmaCubeLarge = create(EntityType::MagmaCubeLarge)
	.iconRowName(TEXT("magmacube"))
	.name(LOCTEXT("mob_MagmaCubeLarge", "Magma Cube"))
;
const MobTypeDef& MagmaCubeMedium = create(EntityType::MagmaCubeMedium)
	.iconRowName(TEXT("magmacube"))
	.name(LOCTEXT("mob_MagmaCubeMedium", "Magma Cube"))
;
const MobTypeDef& MagmaCubeSmall = create(EntityType::MagmaCubeSmall)
	.iconRowName(TEXT("magmacube"))
	.name(LOCTEXT("mob_MagmaCubeSmall", "Magma Cube"))
;
const MobTypeDef& Blaze = create(EntityType::Blaze)
	.name(LOCTEXT("mob_Blaze", "Blaze"))
;
const MobTypeDef& HoveringInfenro = create(EntityType::HoveringInferno)
.name(LOCTEXT("mob_HoveringInferno", "Hovering Inferno"))
;
const MobTypeDef& BlazeSpawner = create(EntityType::BlazeSpawner)
.name(LOCTEXT("mob_BlazeSpawner", "Blaze Spawner"))
;
const MobTypeDef& ZombieVillager = create(EntityType::ZombieVillager)
	.name(LOCTEXT("mob_ZombieVillager", "Zombie Villager"))
;
const MobTypeDef& Witch = create(EntityType::Witch)
	.name(LOCTEXT("mob_Witch", "Witch"))
;
const MobTypeDef& PestilentConjurer = create(EntityType::WitchAncient)
	.iconRowName(EntityType::Witch)
	.name(LOCTEXT("mob_PestilentConjurer", "Pestilent Conjurer"))
;
const MobTypeDef& SoulWizard = create(EntityType::SoulWizard)
	.name(LOCTEXT("mob_SoulWizard", "Soul Wizard"))
;

//These are temporary and only used for mob spawner items
const MobTypeDef& Stray = create(EntityType::Stray)
	.name(LOCTEXT("mob_Stray", "Stray"))
;
const MobTypeDef& Husk = create(EntityType::Husk)
	.name(LOCTEXT("mob_Husk", "Husk"))
;
const MobTypeDef& WitherSkeleton = create(EntityType::WitherSkeleton)
	.name(LOCTEXT("mob_WitherSkeleton", "Wither Skeleton"))
;
const MobTypeDef& WitherSkeletonRanged = create(EntityType::WitherSkeletonRanged)
	.name(LOCTEXT("mob_WitherSkeletonRanged", "Wither Skeleton Archer"))
;
const MobTypeDef& Guardian = create(EntityType::Guardian)
	.name(LOCTEXT("mob_Guardian", "Guardian"))
;
const MobTypeDef& AbyssalEye = create(EntityType::GuardianAncient)
	.iconRowName(EntityType::Guardian)
	.name(LOCTEXT("mob_AbyssalEye", "Abyssal Eye"))
;
const MobTypeDef& ElderGuardian = create(EntityType::ElderGuardian)
	.name(LOCTEXT("mob_ElderGuardian", "Elder Guardian"))
;

const MobTypeDef& Npc = create(EntityType::Npc)
	.name(LOCTEXT("mob_Npc", "Npc"))
;
const MobTypeDef& WitherBoss = create(EntityType::WitherBoss)
	.name(LOCTEXT("mob_WitherBoss", "Wither Boss"))
;
const MobTypeDef& Dragon = create(EntityType::Dragon)
	.name(LOCTEXT("mob_Dragon", "Dragon"))
;
const MobTypeDef& Shulker = create(EntityType::Shulker)
	.name(LOCTEXT("mob_Shulker", "Shulker"))
;
const MobTypeDef& ShulkerRespawning = create(EntityType::ShulkerRespawning)
	.name(LOCTEXT("mob_ShulkerRespawning", "Shulker"))
; 
const MobTypeDef& Endermite = create(EntityType::Endermite)
	.name(LOCTEXT("mob_Endermite", "Endermite"))
;
const MobTypeDef& EndermiteSmart = create(EntityType::EndermiteSmart)
	.name(LOCTEXT("mob_EndermiteSmart", "Endermite"))
;
const MobTypeDef& Agent = create(EntityType::Agent)
	.name(LOCTEXT("mob_Agent", "Agent"))
;

const MobTypeDef& Necromancer = create(EntityType::Necromancer)
	.name(LOCTEXT("mob_Necromancer", "Necromancer"))
;
const MobTypeDef& HauntedCaller = create(EntityType::NecromancerAncient)
	.iconRowName(EntityType::Necromancer)
	.name(LOCTEXT("mob_HauntedCaller", "Haunted Caller"))
;
const MobTypeDef& Vex = create(EntityType::Vex)
	.name(LOCTEXT("mob_Vex", "Vex"))
;
const MobTypeDef& TheSeekingFlame = create(EntityType::VexAncient)
	.iconRowName(EntityType::Vex)
	.name(LOCTEXT("mob_TheSeekingFlame", "The Seeking Flame"))
;
const MobTypeDef& Evoker = create(EntityType::Evoker)
	.name(LOCTEXT("mob_Evoker", "Evoker"))
;
const MobTypeDef& EvokerFang = create(EntityType::EvokerFang)
	.name(LOCTEXT("mob_EvokerFang", "Evoker Fang"))
;
const MobTypeDef& Geomancer = create(EntityType::Geomancer)
	.name(LOCTEXT("mob_Geomancer", "Geomancer"))
;
const MobTypeDef& FrostWarden = create(EntityType::GeomancerAncient)
	.iconRowName(EntityType::Geomancer)
	.name(LOCTEXT("mob_FrostWarden", "FrostWarden"))
;
const MobTypeDef& TripodCamera = create(EntityType::TripodCamera)
	.name(LOCTEXT("mob_TripodCamera", "Tripod Camera"))
;
const MobTypeDef& Player = create(EntityType::Player)
	.name(LOCTEXT("mob_Player", "Player"))
;

const MobTypeDef& GeomancerWall = create(EntityType::GeomancerWall)
	.name(LOCTEXT("mob_GeomancerWall", "Geomancer Wall"))
;
const MobTypeDef& GeomancerBomb = create(EntityType::GeomancerBomb)
	.name(LOCTEXT("mob_GeomancerBomb", "Geomancer Bomb"))
;
const MobTypeDef& ChickenJockey = create(EntityType::ChickenJockey)
	.name(LOCTEXT("mob_ChickenJockey", "Chicken Jockey"))
;
const MobTypeDef& ChickenJockeyTower = create(EntityType::ChickenJockeyTower)
	.iconRowName(EntityType::ChickenJockey)
	.name(LOCTEXT("mob_ChickenJockeyTower", "Chicken Jockey Tower"))
;
const MobTypeDef& TheTower = create(EntityType::ChickenJockeyTowerAncient)
	.iconRowName(EntityType::ChickenJockey)
	.name(LOCTEXT("mob_TheTower", "The Tower"))
;
const MobTypeDef& SlimeLarge = create(EntityType::SlimeLarge)	
	.iconRowName(EntityType::SlimeLarge)
	.name(LOCTEXT("mob_SlimeLarge", "Large Slime"))
;
const MobTypeDef& SlimeMedium = create(EntityType::SlimeMedium)	
	.iconRowName(EntityType::SlimeLarge)
	.name(LOCTEXT("mob_SlimeMedium", "Medium Slime"))
;
const MobTypeDef& SlimeSmall = create(EntityType::SlimeSmall)
	.iconRowName(EntityType::SlimeLarge)
	.name(LOCTEXT("mob_SlimeSmall", "Small Slime"))
;
const MobTypeDef& OozingMenace = create(EntityType::SlimeSmallAncient)
	.iconRowName(EntityType::SlimeLarge)
	.name(LOCTEXT("mob_OozingMenace", "Oozing Menace"))
;
const MobTypeDef& RedstoneMonstrosity = create(EntityType::RedstoneMonstrosity)
	.name(LOCTEXT("mob_RedstoneMonstrosity", "Redstone Monstrosity"))
;
const MobTypeDef& Wraith = create(EntityType::Wraith)
	.name(LOCTEXT("mob_Wraith", "Wraith"))
;
const MobTypeDef& AncientTerror = create(EntityType::WraithAncient)
	.iconRowName(EntityType::Wraith)
	.name(LOCTEXT("mob_AncientTerror ", "Ancient Terror"))
;
const MobTypeDef& RedstoneCube = create(EntityType::RedstoneCube)
	.name(LOCTEXT("mob_RedstoneCube", "Redstone Cube"))
;
const MobTypeDef& PiggyBank = create(EntityType::PiggyBank)
	.name(LOCTEXT("mob_PiggyBank", "Piggy Bank"))
;
const MobTypeDef& GoldBabyKey = create(EntityType::GoldBabyKey)
	.name(LOCTEXT("mob_GoldBabyKey", "Gold Key Golem"))
;
const MobTypeDef& SilverBabyKey = create(EntityType::SilverBabyKey)
	.name(LOCTEXT("mob_SilverBabyKey", "Diamond Key Golem"))
;
const MobTypeDef& NamelessKing = create(EntityType::NamelessKing)
	.name(LOCTEXT("mob_NamelessKing", "Nameless One"))
;
const MobTypeDef& Pillager = create(EntityType::Pillager)
	.name(LOCTEXT("mob_Pillager", "Pillager"))
;
const MobTypeDef& PillagerVariant0 = create(EntityType::PillagerVariant0)
	.iconRowName(EntityType::Pillager)
	.name(LOCTEXT("mob_Pillager", "Pillager"))
;
const MobTypeDef& PillagerVariant1 = create(EntityType::PillagerVariant1)
	.iconRowName(EntityType::Pillager)
	.name(LOCTEXT("mob_Pillager", "Pillager"))
;
const MobTypeDef& PillagerVariant2 = create(EntityType::PillagerVariant2)
	.iconRowName(EntityType::Pillager)
	.name(LOCTEXT("mob_Pillager", "Pillager"))
;
const MobTypeDef& SolemnGiant = create(EntityType::PillagerAncient)
	.iconRowName(EntityType::Pillager)
	.name(LOCTEXT("mob_SolemnGiant", "Solemn Giant"))
;
const MobTypeDef& FalseKing = create(EntityType::FalseKing)
	.name(LOCTEXT("mob_FalseKing", "False King"))
;
const MobTypeDef& SkeletonVanguard = create(EntityType::SkeletonVanguard)
	.name(LOCTEXT("mob_SkeletonVanguard", "Vanguard"))
;
const MobTypeDef& CursedPresence = create(EntityType::SkeletonVanguardAncient)
	.iconRowName(EntityType::SkeletonVanguard)
	.name(LOCTEXT("mob_CursedPresence", "Cursed Presence"))
;
const MobTypeDef& MobSpawner = create(EntityType::MobSpawner)
	.name(LOCTEXT("mob_MobSpawner", "Mob Spawner"))
;
const MobTypeDef& TheUnending = create(EntityType::BlazeSpawnerAncient)
	.iconRowName(EntityType::MobSpawner)
	.name(LOCTEXT("mob_TheUnending", "The Unending"))
;
const MobTypeDef& SkeletonHorseman = create(EntityType::SkeletonHorseman)
	.name(LOCTEXT("mob_SkeletonHorseman", "Skeleton Horseman"))
;
const MobTypeDef& OrdinaryHorse = create(EntityType::OrdinaryHorse)
	.name(LOCTEXT("mob_OrdinaryHorse", "Ordinary Horse"))
;
const MobTypeDef& CauldronBoss = create(EntityType::CauldronBoss)
	.name(LOCTEXT("mob_CauldronBoss", "Corrupted Cauldron"))
;
const MobTypeDef& SlimeCauldron = create(EntityType::SlimeCauldron)
	.name(LOCTEXT("mob_SlimeCauldron", "Pink Slime"))
;
const MobTypeDef& ArchIllager = create(EntityType::ArchIllager)
	.name(LOCTEXT("mob_ArchIllager", "Arch-Illager"))
;
const MobTypeDef& ArchVessel = create(EntityType::ArchVessel)
	.name(LOCTEXT("mob_ArchVessel", "Heart of Ender"))
;
const MobTypeDef& ArchVisage = create(EntityType::ArchVisage)
	.name(LOCTEXT("mob_ArchVisage", "Arch-Visage"))
;
const MobTypeDef& JackOLantern = create(EntityType::JackOLantern)
	.name(LOCTEXT("mob_JackOLantern", "Jack O' Lantern"))
;
const MobTypeDef& RoyalGuard = create(EntityType::RoyalGuard)
	.name(LOCTEXT("mob_RoyalGuard", "Royal Guard"))
;
const MobTypeDef& VigilantScoundrel = create(EntityType::RoyalGuardAncient)
	.iconRowName(EntityType::RoyalGuard)
	.name(LOCTEXT("mob_VigilantScoundrel", "Vigilant Scoundrel"))
;
const MobTypeDef& VindicatorChef = create(EntityType::VindicatorChef)
	.iconRowName(EntityType::Vindicator)
	.name(LOCTEXT("mob_VindicatorChef", "Vindicator Chef"))
;
const MobTypeDef& Llama = create(EntityType::Llama)
	.name(LOCTEXT("mob_Llama", "Llama"))
;
const MobTypeDef& LlamaMob = create(EntityType::LlamaMob)
.name(LOCTEXT("mob_LlamaMob", "Llama"))
;

const MobTypeDef& Enderman = create(EntityType::Enderman)
	.name(LOCTEXT("mob_Enderman", "Enderman"))
;

const MobTypeDef& MinecartHopper = create(EntityType::MinecartHopper)
	.name(LOCTEXT("mob_MinecartHopper", "Minecart Hopper"))
;
const MobTypeDef& MinecartTNT = create(EntityType::MinecartTNT)
	.name(LOCTEXT("mob_MinecartTNT", "Minecart TNT"))
;
const MobTypeDef& MinecartChest = create(EntityType::MinecartChest)
	.name(LOCTEXT("mob_MinecartChest", "Minecart Chest"))
;
const MobTypeDef& MinecartFurnace = create(EntityType::MinecartFurnace)
	.name(LOCTEXT("mob_MinecartFurnace", "Minecart Furnace"))
;
const MobTypeDef& MooshroomMonstrosity = create(EntityType::MooshroomMonstrosity)
	.name(LOCTEXT("mob_MooshroomMonstrosity", "Mooshroom Monstrosity"))
;
const MobTypeDef& BabyChicken = create(EntityType::BabyChicken)
	.name(LOCTEXT("mob_BabyChicken", "Baby Chicken"))
;
const MobTypeDef& BabyPig = create(EntityType::BabyPig)
.name(LOCTEXT("mob_BabyPig", "Baby Pig"))
;

// D11 Mobs;
const MobTypeDef& JungleZombie = create(EntityType::JungleZombie)
	.name(LOCTEXT("mob_JungleZombie", "Jungle Zombie"))
;
const MobTypeDef& MossySkeleton = create(EntityType::MossySkeleton)
	.name(LOCTEXT("mob_MossySkeleton", "Mossy Skeleton"))
;
const MobTypeDef& QuickGrowingVine = create(EntityType::QuickGrowingVine)
	.name(LOCTEXT("mob_QuickGrowingVine", "Quick Growing Vine"))
;
const MobTypeDef& PoisonQuillVine = create(EntityType::PoisonQuillVine)
	.name(LOCTEXT("mob_PoisonQuillVine", "Poison Quill Vine"))
;
const MobTypeDef& Ocelot = create(EntityType::Ocelot)
	.name(LOCTEXT("mob_Ocelot", "Ocelot"))
;
const MobTypeDef& PlayfulPanda = create(EntityType::PlayfulPanda)
	.name(LOCTEXT("mob_PlayfulPanda", "Playful Panda"))
;
const MobTypeDef& LazyPanda = create(EntityType::LazyPanda)
	.name(LOCTEXT("mob_LazyPanda", "Lazy Panda"))
;
const MobTypeDef& BrownPanda = create(EntityType::BrownPanda)
	.name(LOCTEXT("mob_BrownPanda", "Brown Panda"))
;
const MobTypeDef& Leaper = create(EntityType::Leaper)
	.name(LOCTEXT("mob_Leaper", "Leapleaf"))
;
const MobTypeDef& ThunderingGrowth = create(EntityType::LeaperAncient)
	.iconRowName(EntityType::Leaper)
	.name(LOCTEXT("mob_ThunderingGrowth", "Thundering Growth"))
;
const MobTypeDef& Whisperer = create(EntityType::Whisperer)
	.name(LOCTEXT("mob_Whisperer", "Whisperer"))
;
const MobTypeDef& WaveWhisperer = create(EntityType::WaveWhisperer)
.name(LOCTEXT("mob_WaveWhisperer", "WaveWhisperer"))
;
const MobTypeDef& JungleAbomination = create(EntityType::JungleAbomination)
	.name(LOCTEXT("mob_JungleAbomination", "Jungle Abomination"))
;
const MobTypeDef& QuickGrowingVineSimple = create(EntityType::QuickGrowingVineSimple)
	.name(LOCTEXT("mob_QuickGrowingVineSimple", "Quick Growing Vine"))
;
const MobTypeDef& PoisonQuillVineSimple = create(EntityType::PoisonQuillVineSimple)
	.name(LOCTEXT("mob_PoisonQuillVineSimple", "Poison Quill Vine"))
;
const MobTypeDef& QuickGrowingKelp = create(EntityType::QuickGrowingKelp)
.name(LOCTEXT("mob_QuickGrowingKelp", "Quick Growing Kelp"))
;
const MobTypeDef& PoisonAnemone = create(EntityType::PoisonAnemone)
.name(LOCTEXT("mob_PoisonAnemone", "Poison Anemone"))
;
const MobTypeDef& EntangleVine = create(EntityType::EntangleVine)
	.name(LOCTEXT("mob_EntangleVine", "Entangle Vine"))
;
const MobTypeDef& AbominationVine = create(EntityType::AbominationVine)
	.name(LOCTEXT("mob_AbominationVine", "Abomination Vine"))
;
const MobTypeDef& BabyPanda = create(EntityType::BabyPanda)
	.name(LOCTEXT("mob_BabyPanda", "Baby Panda"))
;
const MobTypeDef& Parrot = create(EntityType::Parrot)
	.name(LOCTEXT("mob_Parrot", "Parrot"))
;
const MobTypeDef& Panda = create(EntityType::Panda)
	.name(LOCTEXT("mob_Panda", "Panda"))
;
const MobTypeDef& FrozenZombie = create(EntityType::FrozenZombie)
	.name(LOCTEXT("mob_FrozenZombie", "Frozen Zombie"))
;
const MobTypeDef& IcyCreeper = create(EntityType::IcyCreeper)
	.name(LOCTEXT("mob_IcyCreeper", "Icy Creeper"))
;
const MobTypeDef& ArcticFox = create(EntityType::ArcticFox)
	.name(LOCTEXT("mob_ArcticFox", "Arctic Fox"))
;
const MobTypeDef& Illusioner = create(EntityType::Illusioner)
	.name(LOCTEXT("mob_Illusioner", "Illusioner"))
;
const MobTypeDef& IllusionerClone = create(EntityType::IllusionerClone)
	.name(LOCTEXT("mob_IllusionerClone", "Illusioner"))
;
const MobTypeDef& Chillager = create(EntityType::Chillager)
	.name(LOCTEXT("mob_Chillager", "Iceologer"))
;
const MobTypeDef& WickedWraith = create(EntityType::WickedWraith)
	.name(LOCTEXT("mob_WickedWraith", "Wretched Wraith"))
;
const MobTypeDef& Hoglin = create(EntityType::Hoglin)
	.name(LOCTEXT("mob_Hoglin", "Hoglin"))
;
const MobTypeDef& UnstoppableTusk = create(EntityType::HoglinAncient)
	.iconRowName(EntityType::Hoglin)
	.name(LOCTEXT("mob_UnstoppableTusk", "Unstoppable Tusk"))
;
const MobTypeDef& PiglinFungusThrower = create(EntityType::PiglinFungusThrower)
.name(LOCTEXT("mob_PiglinFungusThrower", "Fungus Thrower"))
;
const MobTypeDef& ZombifiedPiglinFungusThrower = create(EntityType::ZombifiedPiglinFungusThrower)
.iconRowName(EntityType::PiglinFungusThrower)
.name(LOCTEXT("mob_ZombifiedPiglinFungusThrower", "Zombified Fungus Thrower"))
;
const MobTypeDef& PiglinMelee = create(EntityType::PiglinMelee)
	.name(LOCTEXT("mob_PiglinMelee", "Piglin"))
;
const MobTypeDef& PiglinMeleeVariant0 = create(EntityType::PiglinMeleeVariant0)
	.iconRowName(EntityType::PiglinMelee)
	.name(LOCTEXT("mob_PiglinMelee", "Piglin"))
;
const MobTypeDef& PiglinMeleeVariant1 = create(EntityType::PiglinMeleeVariant1)
	.iconRowName(EntityType::PiglinMelee)
	.name(LOCTEXT("mob_PiglinMelee", "Piglin"))
;
const MobTypeDef& PiglinMeleeVariant2 = create(EntityType::PiglinMeleeVariant2)
	.iconRowName(EntityType::PiglinMelee)
	.name(LOCTEXT("mob_PiglinMelee", "Piglin"))
;
const MobTypeDef& PiglinHunter = create(EntityType::PiglinRanged)
	.name(LOCTEXT("mob_PiglinRanged", "Piglin Hunter"))
;
const MobTypeDef& PiglinHunterVariant0 = create(EntityType::PiglinRangedVariant0)
	.name(LOCTEXT("mob_PiglinRanged", "Piglin Hunter"))
;
const MobTypeDef& PiglinHunterVariant1 = create(EntityType::PiglinRangedVariant1)
	.name(LOCTEXT("mob_PiglinRanged", "Piglin Hunter"))
;
const MobTypeDef& PiglinHunterVariant2 = create(EntityType::PiglinRangedVariant2)
	.name(LOCTEXT("mob_PiglinRanged", "Piglin Hunter"))
;
const MobTypeDef& ZombifiedPiglinMelee = create(EntityType::ZombifiedPiglinMelee)
	.iconRowName(EntityType::PiglinMelee)
	.name(LOCTEXT("mob_ZombifiedPiglinMelee", "Zombified Piglin"))
;
const MobTypeDef& ZombifiedPiglinMeleeVariant0 = create(EntityType::ZombifiedPiglinMeleeVariant0)
	.iconRowName(EntityType::PiglinMelee)
	.name(LOCTEXT("mob_ZombifiedPiglinMelee", "Zombified Piglin"))
;
const MobTypeDef& ZombifiedPiglinMeleeVariant1 = create(EntityType::ZombifiedPiglinMeleeVariant1)
	.iconRowName(EntityType::PiglinMelee)
	.name(LOCTEXT("mob_ZombifiedPiglinMelee", "Zombified Piglin"))
;
const MobTypeDef& ZombifiedPiglinMeleeVariant2 = create(EntityType::ZombifiedPiglinMeleeVariant2)
	.iconRowName(EntityType::PiglinMelee)
	.name(LOCTEXT("mob_ZombifiedPiglinMelee", "Zombified Piglin"))
;
const MobTypeDef& ZombifiedPiglinRanged = create(EntityType::ZombifiedPiglinRanged)
	.iconRowName(EntityType::PiglinRanged)
	.name(LOCTEXT("mob_ZombifiedPiglinRanged", "Zombified Piglin Hunter"))
;
const MobTypeDef& ZombifiedPiglinRangedVariant0 = create(EntityType::ZombifiedPiglinRangedVariant0)
	.iconRowName(EntityType::PiglinRanged)
	.name(LOCTEXT("mob_ZombifiedPiglinRanged", "Zombified Piglin Hunter"))
;
const MobTypeDef& ZombifiedPiglinRangedVariant1 = create(EntityType::ZombifiedPiglinRangedVariant1)
	.iconRowName(EntityType::PiglinRanged)
	.name(LOCTEXT("mob_ZombifiedPiglinRanged", "Zombified Piglin Hunter"))
;
const MobTypeDef& ZombifiedPiglinRangedVariant2 = create(EntityType::ZombifiedPiglinRangedVariant2)
	.iconRowName(EntityType::PiglinRanged)
	.name(LOCTEXT("mob_ZombifiedPiglinRanged", "Zombified Piglin Hunter"))
;

const MobTypeDef& RampartCaptain = create(EntityType::RampartCaptain)
	.name(LOCTEXT("mob_RampartCaptain", "Rampart Captain"))
;


//Patch2 mobs

const MobTypeDef& Bee = create(EntityType::Bee)
.name(LOCTEXT("mob_Bee", "Bee"));

const MobTypeDef& TropicalSlimeLarge = create(EntityType::TropicalSlimeLarge)
.name(LOCTEXT("mob_TropicalSlimeLarge", "Large Tropical Slime"))
;
const MobTypeDef& TropicalSlimeMedium = create(EntityType::TropicalSlimeMedium)
.name(LOCTEXT("mob_TropicalSlimeMedium", "Medium Tropical Slime"))
;
const MobTypeDef& TropicalSlimeSmall = create(EntityType::TropicalSlimeSmall)
.name(LOCTEXT("mob_TropicalSlimeSmall", "Small Tropical Slime"))
;
const MobTypeDef& Turtle = create(EntityType::Turtle)
.name(LOCTEXT("mob_turtle", "Turtle"))
;
const MobTypeDef& BabyTurtle = create(EntityType::BabyTurtle)
.name(LOCTEXT("mob_BabyTurtle", "Baby Turtle"))
;
const MobTypeDef& Pufferfish = create(EntityType::Pufferfish)
.name(LOCTEXT("mob_Pufferfish", "Pufferfish"))
;
const MobTypeDef& Drowned = create(EntityType::Drowned)
.name(LOCTEXT("mob_Drowned", "Drowned"))
;
const MobTypeDef& VengefulMariner = create(EntityType::DrownedAncient)
.iconRowName(EntityType::Drowned)
.name(LOCTEXT("mob_VengefulMariner", "Vengeful Mariner"))
;
const MobTypeDef& DrownedVariant0 = create(EntityType::DrownedVariant0)
.name(LOCTEXT("mob_DrownedVariant0", "Drowned"))
;
const MobTypeDef& DrownedVariant1 = create(EntityType::DrownedVariant1)
.name(LOCTEXT("mob_DrownedVariant1", "Drowned"))
;
const MobTypeDef& DrownedVariant2 = create(EntityType::DrownedVariant2)
.name(LOCTEXT("mob_DrownedVariant2", "Drowned"))
;
const MobTypeDef& TridentDrowned = create(EntityType::TridentDrowned)
.name(LOCTEXT("mob_TridentDrowned", "Trident Drowned"))
;
const MobTypeDef& TridentDrownedVariant0 = create(EntityType::TridentDrownedVariant0)
.name(LOCTEXT("mob_TridentDrownedVariant0", "Trident Drowned"))
;
const MobTypeDef& TridentDrownedVariant1 = create(EntityType::TridentDrownedVariant1)
.name(LOCTEXT("mob_TridentDrownedVariant1", "Trident Drowned"))
;
const MobTypeDef& TridentDrownedVariant2 = create(EntityType::TridentDrownedVariant2)
.name(LOCTEXT("mob_TridentDrownedVariant2", "Trident Drowned"))
;
const MobTypeDef& Dolphin = create(EntityType::Dolphin)
.name(LOCTEXT("mob_Dolphin", "Dolphin"))
;
const MobTypeDef& SunkenSkeleton = create(EntityType::SunkenSkeleton)
.name(LOCTEXT("mob_SunkenSkeleton", "Sunken Skeleton"))
;
const MobTypeDef& SunkenSkeletonVariant0 = create(EntityType::SunkenSkeletonVariant0)
.name(LOCTEXT("mob_SunkenSkeletonVariant0", "Sunken Skeleton"))
;
const MobTypeDef& SunkenSkeletonVariant1 = create(EntityType::SunkenSkeletonVariant1)
.name(LOCTEXT("mob_SunkenSkeletonVariant1", "Sunken Skeleton"))
;
const MobTypeDef& SunkenSkeletonVariant2 = create(EntityType::SunkenSkeletonVariant2)
.name(LOCTEXT("mob_SunkenSkeletonVariant2", "Sunken Skeleton"))
;
const MobTypeDef& DrownedNecromancer = create(EntityType::DrownedNecromancer)
.name(LOCTEXT("mob_DrownedNecromancer", "Drowned Necromancer"))
;
const MobTypeDef& BabyTurtlePet = create(EntityType::BabyTurtlePet)
.name(LOCTEXT("mob_BabyTurtlePet", "Baby Turtle"))
;
const MobTypeDef& VindicatorRaidCaptain = create(EntityType::VindicatorRaidCaptain)
.name(LOCTEXT("mob_VindicatorRaidCaptain", "Vindicator Raid Captain"))
;
const MobTypeDef& PillagerRaidCaptain = create(EntityType::PillagerRaidCaptain)
.name(LOCTEXT("mob_PillagerRaidCaptain", "Pillager Raid Captain"))
;
const MobTypeDef& AncientGuardian = create(EntityType::AncientGuardian)
.name(LOCTEXT("mob_AncientGuardian", "Ancient Guardian"))
;
const MobTypeDef& Biomine = create(EntityType::Biomine)
.name(LOCTEXT("mob_Biomine", "Biomine"))
;

// DLC 6
const MobTypeDef& PerfectFormHeart = create(EntityType::PerfectFormHeart)
.name(LOCTEXT("mob_PerfectFormHeart", "Vengeful Heart of Ender"))
;
const MobTypeDef& EndermiteMinion = create(EntityType::EndermiteMinion)
.name(LOCTEXT("mob_EndermiteMinion", "Endermite"))
;
const MobTypeDef& Blastling = create(EntityType::Blastling)
.name(LOCTEXT("mob_Blastling", "Blastling"))
;
const MobTypeDef& Snareling = create(EntityType::Snareling)
.name(LOCTEXT("mob_Snareling", "Snareling"))
;
const MobTypeDef& TheSwarm = create(EntityType::SnarelingAncient)
.iconRowName(EntityType::Snareling)
.name(LOCTEXT("mob_TheSwarm", "The Swarm"))
;
const MobTypeDef& Endling = create(EntityType::Endling)
.name(LOCTEXT("mob_Endling", "Watchling"))
;
const MobTypeDef& WatcherOfTheEnd = create(EntityType::EndlingAncient)
.iconRowName(EntityType::Endling)
.name(LOCTEXT("mob_WatcherOfTheEnd", "Watcher of the End"))
;
const MobTypeDef& Endersent = create(EntityType::Endersent)
.name(LOCTEXT("mob_Endersent", "Endersent"))
;
const MobTypeDef& Thornblight = create(EntityType::EndersentThornblight)
.name(LOCTEXT("mob_Thornblight", "Spiked Eye"))
;
const MobTypeDef& Voidstrike = create(EntityType::EndersentVoidstrike)
.name(LOCTEXT("mob_Voidstrike", "Savage Eye"))
;
const MobTypeDef& Writherot = create(EntityType::EndersentWritherot)
.name(LOCTEXT("mob_Writherot", "Blight Eye"))
;
const MobTypeDef& Everfire = create(EntityType::EndersentEverfire)
.name(LOCTEXT("mob_Everfire", "Binding Eye"))
;
const MobTypeDef& Surgefiend = create(EntityType::EndersentSurgefiend)
.name(LOCTEXT("mob_Surgefiend", "Reaping Eye"))
;
const MobTypeDef& Deadeye = create(EntityType::EndersentDeadeye)
.name(LOCTEXT("mob_Deadeye", "Ravenous Eye"))
;
const MobTypeDef& Seravex = create(EntityType::Seravex)
.name(LOCTEXT("mob_Seravex", "Guardian Vex"))
;

const MobTypeDef& Friendermite = create(EntityType::Friendermite)
.name(LOCTEXT("mob_Friendermite", "Endermite"));
//////////////////////////////////////////////////////////////////////////

//
// Code getters
//

const FText& getMobDisplayName(EntityType mobType) {
	if (auto mobDef = getChecked(mobType)) {
		return mobDef->GetName();
	}

	return FText::GetEmpty();
}

const MobTypeDef& get(EntityType mobTypeName) {
	if (auto mobType = getChecked(mobTypeName)) {
		return *mobType;
	}
	return Invalid;
}

const MobTypeDef* getChecked(EntityType mobTypeName) {
	if (mobTypeDefs.Contains(mobTypeName)) { 
		return mobTypeDefs[mobTypeName].get();
	}
	return nullptr;
}


const TArray<EntityType> getAllEnabled() {
	TArray<EntityType> all;
	for (auto&& mutableDef : mob::type::mobTypeDefs) {
		MobTypeDef* mobTypeDef = mutableDef.Value.get();
		if (mobTypeDef && !mobTypeDef->IsDisabled()) {
			all.Add(mobTypeDef->mobType());
		}
	}
	return all;
}

}}


#undef LOCTEXT_NAMESPACE
