#include "Dungeons.h"
#include "MobTags.h"
#include "EntityClassTree.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "util/StringUtils.h"
#include "util/Algo.h"

const std::string
MobTags::Friendly = "friendly",
MobTags::Animal = "animal",
MobTags::Monster = "monster",
MobTags::Melee = "melee",
MobTags::Ranged = "ranged",
MobTags::Disable = "disable",
MobTags::Aoe = "aoe",
MobTags::Fast = "fast",
MobTags::Baby = "baby",
MobTags::Weak = "weak",
MobTags::Strong = "strong",
MobTags::Miniboss = "miniboss",
MobTags::Centerpiece = "centerpiece",
MobTags::Caster = "caster",
MobTags::Summoner = "summoner",
MobTags::Undead = "undead",
MobTags::Passive = "passive",
MobTags::DLC = "dlc",
MobTags::EventMob = "eventmob",
MobTags::Immobile = "immobile",
MobTags::Unlovable = "Unlovable",
MobTags::Floating = "floating",
MobTags::LowPriority = "lowpriority",
MobTags::Special = "special",
MobTags::Unenchantable = "unenchantable",
MobTags::Pet = "pet",
MobTags::Illager = "illager",
MobTags::NoRes = "nores",
MobTags::MountainAnimal = "mountainanimal",
MobTags::OceanAnimal = "oceananimal",
MobTags::Ancient = "ancient",
MobTags::Underwater = "underwater",
MobTags::WinterAnimal = "winteranimal",
MobTags::JungleAnimal = "jungleanimal",
MobTags::BluffAnimal = "bluffanimal",
MobTags::Cosmetic = "cosmetic",
MobTags::Ender		= "ender",
MobTags::NoEnchantedName = "noenchantedname",
MobTags::Unbubbled = "unbubbled",
MobTags::RaidCaptain = "raidcaptain",
MobTags::NoHyperMission = "NoHyperMission",
MobTags::Unchainable = "Unchainable";

const size_t
MobTags::HashTag_Friendly = std::hash<std::string>{}(MobTags::Friendly),
MobTags::HashTag_Animal = std::hash<std::string>{}(MobTags::Animal),
MobTags::HashTag_Monster = std::hash<std::string>{}(MobTags::Monster),
MobTags::HashTag_Melee = std::hash<std::string>{}(MobTags::Melee),
MobTags::HashTag_Ranged = std::hash<std::string>{}(MobTags::Ranged),
MobTags::HashTag_Disable = std::hash<std::string>{}(MobTags::Disable),
MobTags::HashTag_Aoe = std::hash<std::string>{}(MobTags::Aoe),
MobTags::HashTag_Fast = std::hash<std::string>{}(MobTags::Fast),
MobTags::HashTag_Baby = std::hash<std::string>{}(MobTags::Baby),
MobTags::HashTag_Weak = std::hash<std::string>{}(MobTags::Weak),
MobTags::HashTag_Strong = std::hash<std::string>{}(MobTags::Strong),
MobTags::HashTag_Miniboss = std::hash<std::string>{}(MobTags::Miniboss),
MobTags::HashTag_Centerpiece = std::hash<std::string>{}(MobTags::Centerpiece),
MobTags::HashTag_Caster = std::hash<std::string>{}(MobTags::Caster),
MobTags::HashTag_Summoner = std::hash<std::string>{}(MobTags::Summoner),
MobTags::HashTag_Undead = std::hash<std::string>{}(MobTags::Undead),
MobTags::HashTag_Passive = std::hash<std::string>{}(MobTags::Passive),
MobTags::HashTag_DLC = std::hash<std::string>{}(MobTags::DLC),
MobTags::HashTag_EventMob = std::hash<std::string>{}(MobTags::EventMob),
MobTags::HashTag_Immobile = std::hash<std::string>{}(MobTags::Immobile),
MobTags::HashTag_Unlovable = std::hash<std::string>{}(MobTags::Unlovable),
MobTags::HashTag_Floating = std::hash<std::string>{}(MobTags::Floating),
MobTags::HashTag_LowPriority = std::hash<std::string>{}(MobTags::LowPriority),
MobTags::HashTag_Special = std::hash<std::string>{}(MobTags::Special),
MobTags::HashTag_Unenchantable = std::hash<std::string>{}(MobTags::Unenchantable),
MobTags::HashTag_Pet = std::hash<std::string>{}(MobTags::Pet),
MobTags::HashTag_MountainAnimal = std::hash<std::string>{}(MobTags::MountainAnimal),
MobTags::HashTag_OceanAnimal = std::hash<std::string>{}(MobTags::OceanAnimal),
MobTags::HashTag_Illager = std::hash<std::string>{}(MobTags::Illager),
MobTags::HashTag_Ancient = std::hash<std::string>{}(MobTags::Ancient),
MobTags::HashTag_Underwater = std::hash<std::string>{}(MobTags::Underwater),
MobTags::HashTag_WinterAnimal = std::hash<std::string>{}(MobTags::WinterAnimal),
MobTags::HashTag_JungleAnimal = std::hash<std::string>{}(MobTags::JungleAnimal),
MobTags::HashTag_BluffAnimal = std::hash<std::string>{}(MobTags::BluffAnimal),
MobTags::HashTag_Cosmetic = std::hash<std::string>{}(MobTags::Cosmetic),
MobTags::HashTag_Ender = std::hash<std::string>{}(MobTags::Ender),
MobTags::HasTag_NoEnchantedName = std::hash<std::string>{}(MobTags::NoEnchantedName),
MobTags::HashTag_Unbubbled = std::hash<std::string>{}(MobTags::Unbubbled),
MobTags::HashTag_RaidCaptain = std::hash<std::string>{}(MobTags::RaidCaptain),
MobTags::HashTag_NoRes = std::hash<std::string>{}(MobTags::NoRes),
MobTags::HashTag_NoHyperMission = std::hash<std::string>{}(MobTags::NoHyperMission),
MobTags::HashTag_Unchainable = std::hash<std::string>{}(MobTags::Unchainable);

MobTags::MobTags() {
	tag(EntityType::BabyZombie,
		{ 0, 0, 0 },
		{ Fast, Baby, Melee }
	);

	tag(EntityType::BabyDrowned,
		{ 0, 0, 0 },
		{ Fast, Baby, Melee, Underwater }
	);

	tag(EntityType::BabyZombieAncient,
		{ 0, 0, 0 },
		{ Fast, Baby, Melee, Ancient, Special }
	);

	tag(EntityType::CaveSpider,
		{ 0, 2, 1 },
		{ Melee, Strong }
	);

	tag(EntityType::Creeper,
		{ 0, 0.75f, 0.75f },
		{ Melee, Aoe }
	);

	tag(EntityType::ChargedCreeper,
		{ 0, 0.75f, 0.75f },
		{ Melee, Aoe }
	);

	tag(EntityType::Husk,
		{ 0, 1, 1 },
		{ Melee, Weak, Strong }
	);

	tag(EntityType::Mooshroom,
		{ 0, 1, 1 },
		{ Melee, Weak, Strong }
	);

	tag(EntityType::MooshroomAncient,
		{ 0, 1, 1 },
		{ Melee, Weak, Strong, Ancient, Special }
	);

	tag(EntityType::IronGolem,
		{ 0, 1, 1 },
		{ Friendly, Strong, Pet }
	);

	tag(EntityType::Necromancer,
		{ 0, 1, 1 },
		{ Strong, Summoner, Ranged }
	);

	tag(EntityType::NecromancerAncient,
		{ 0, 1, 1 },
		{ Strong, Summoner, Ranged, Ancient, Special }
	);

	tag(EntityType::RedstoneGolem,
		{ 0, 1, 1 },
		{ Miniboss, EventMob, Special }
	);

	tag(EntityType::RedstoneGolemAncient,
		{ 0, 1, 1 },
		{ Ancient, Special }
	);

	tag(EntityType::RedstoneMonstrosity,
		{ 0, 1, 1 },
		{ Miniboss, Special }
	);

	tag(EntityType::MooshroomMonstrosity,
		{ 0, 1, 1 },
		{ Miniboss, Special }
	);

	tag(EntityType::RedstoneCube,
		{ 0, 1, 1 },
		{ Weak, Special, NoRes }
	);

	tag(EntityType::Enchanter,
		{ 0, 1, 1 },
		{ Weak, Ranged, Caster, Illager }
	);

	tag(EntityType::EnchanterAncient,
		{ 0, 1, 1 },
		{ Weak, Ranged, Caster, Illager, Ancient, Special }
	);

	tag(EntityType::Vindicator,
		{ 0, 1, 1 },
		{ Strong, Melee, Illager }
	);

	tag(EntityType::Vex,
		{ 0, 1, 1 },
		{ Floating, Special, NoRes }
	);

	tag(EntityType::VexAncient,
		{ 0, 1, 1 },
		{ Floating, Special, Ancient, Special }
	);

	tag(EntityType::Wraith,
		{ 0, 1, 1 },
		{ Melee, Ranged, Caster, Floating }
	);

	tag(EntityType::WraithAncient,
		{ 0, 1, 1 },
		{ Melee, Ranged, Caster, Floating, Ancient, Special }
	);

	tag(EntityType::Evoker,
		{ 0, 1, 1 },
		{ Strong, Summoner, Ranged, Miniboss, EventMob, Special }
	);

	tag(EntityType::EvokerFang,
		{ 0, 1, 1},
		{ Unenchantable, NoRes }
	);

	tag(EntityType::Geomancer,
		{ 0, 1, 1 },
		{ Ranged, Caster, Illager }
	);

	tag(EntityType::GeomancerAncient,
		{ 0, 1, 1 },
		{ Ranged, Caster, Illager, Ancient, Special }
	);

	tag(EntityType::GeomancerWall,
		{ 0, 1, 1 },
		{ Unenchantable, NoRes, Unbubbled }
	);

	tag(EntityType::GeomancerBomb,
		{ 0, 1, 1 },
		{ Unenchantable, NoRes, Unbubbled }
	);

	tag(EntityType::ChickenJockey,
		{ 0, 1, 1 },
		{ Melee, Fast, Strong }
	);

	tag(EntityType::SlimeLarge,
		{ 0, 1, 1 },
		{ Melee, Strong }
	);

	tag(EntityType::SlimeMedium,
		{ 0, 1, 1 },
		{ Melee }
	);

	tag(EntityType::SlimeSmall,
		{ 0, 1, 1 },
		{ Melee, Weak }
	);

	tag(EntityType::SlimeSmallAncient,
		{ 0, 1, 1 },
		{ Melee, Ancient, Special }
	);

	tag(EntityType::MagmaCubeLarge,
		{ 0, 1, 1 },
		{ Melee }
	);

	tag(EntityType::MagmaCubeMedium,
		{ 0, 1, 1 },
		{ Melee }
	);

	tag(EntityType::MagmaCubeSmall,
		{ 0, 1, 1 },
		{ Melee, Weak }
	);

	tag(EntityType::Silverfish,
		{ 0, 0, 0 },
		{ Melee, Weak }
	);

	tag(EntityType::SilverfishSmart,
		{ 0, 0, 0 },
		{ Melee, Weak }
	);

	tag(EntityType::Endermite,
		{ 0, 0, 0 },
		{ Melee, Weak, Ender }
	);

	tag(EntityType::EndermiteSmart,
		{ 0, 0, 0 },
		{ Melee, Weak, Ender }
	);

	tag(EntityType::SilverfishAncient,
		{ 0, 0, 0 },
		{ Ancient, Special }
	);

	tag(EntityType::Skeleton,
		{ 0, 0, 0 },
		{ Ranged, Weak }
	);

	tag(EntityType::Blaze,
		{ 0, 0, 0 },
		{ Ranged, Weak }
	);

	tag(EntityType::HoveringInferno,
		{ 0, 0, 0 },
		{ Ranged, Strong }
	);

	tag(EntityType::BlazeSpawner,
		{ 0, 0, 0 },
		{ Summoner, Strong, Miniboss }
	);

	tag(EntityType::BlazeSpawnerAncient,
		{ 0, 0, 0 },
		{ Summoner, Strong, Immobile, Ancient, Special }
	);

	tag(EntityType::WitherSkeleton,
		{ 0, 0, 0 },
		{ Melee }
	);

	tag(EntityType::WitherSkeletonRanged,
		{ 0, 0, 0 },
		{ Ranged }
	);

	tag(EntityType::PiglinFungusThrower,
		{ 0, 0, 0 },
		{ Ranged }
	);

	tag(EntityType::ZombifiedPiglinFungusThrower,
		{ 0, 0, 0 },
		{ Ranged }
	);

	tag(EntityType::Spider,
		{ 0, 0, 0 },
		{ Melee, Ranged, Disable }
	);

	tag(EntityType::SpiderAncient,
		{ 0, 0, 0 },
		{ Melee, Ranged, Disable, Ancient, Special }
	);

	tag(EntityType::Witch,
		{ 0, 1.25f, 0 },
		{ Ranged, Aoe, Strong, Caster }
	);

	tag(EntityType::WitchAncient,
		{ 0, 1.25f, 0 },
		{ Ranged, Aoe, Strong, Caster, Ancient, Special }
	);

	tag(EntityType::Zombie,
		{ 0, 0, 0 },
		{ Melee, Weak }
	);

	tag(EntityType::PiggyBank,
		{ 0, 0, 0 },
		{ Friendly, Weak, NoRes }
	);

	tag(EntityType::GoldBabyKey,
		{ 0, 0, 0 },
		{ Friendly, Weak, Unenchantable, NoRes }
	);

	tag(EntityType::NamelessKing,
		{ 0, 0, 0 },
		{ Strong, Summoner, Ranged, Miniboss, Special }
	);

	tag(EntityType::SilverBabyKey,
		{ 0, 0, 0 },
		{ Friendly, Weak, Unenchantable, NoRes }
	);

	tag(EntityType::Pillager,
		{ 0, 0, 0 },
		{ Strong, Ranged, Illager }
	);

	tag(EntityType::FalseKing,
		{ 0, 0, 0 },
		{ Weak, NoRes }
	);

	tag(EntityType::SkeletonVanguard,
		{ 0, 0, 0 },
		{ Strong, Melee }
	);

	tag(EntityType::SkeletonVanguardAncient,
		{ 0, 0, 0 },
		{ Strong, Melee, Ancient, Special }
	);

	tag(EntityType::Wolf,
		{ 0, 0, 0 },
		{ Strong, Friendly, Fast, Pet }
	);

	tag(EntityType::SheepFireRed,
		{ 0, 0, 0 },
		{ Strong, Friendly, Fast, Pet }
	);

	tag(EntityType::SheepPoisonGreen,
		{ 0, 0, 0 },
		{ Strong, Friendly, Fast, Pet }
	);

	tag(EntityType::SheepSpeedBlue,
		{ 0, 0, 0 },
		{ Strong, Friendly, Fast, Pet }
	);

	tag(EntityType::MobSpawner,
		{ 0, 0, 0 },
		{ Summoner, Strong, Miniboss }
	);

	tag(EntityType::Rabbit,
		{ 0, 0, 0 },
		{ Weak, Passive, WinterAnimal, DLC }
	);

	tag(EntityType::Hoglin,
		{ 0, 0, 0 },
		{ Melee, Fast, Strong }
	);

	tag(EntityType::HoglinAncient,
		{ 0, 0, 0 },
		{ Melee, Fast, Strong, Ancient, Special }
	);

	tag(EntityType::SkeletonHorseman,
		{ 0, 0, 0 },
		{ Ranged, Fast, Miniboss, EventMob, Special }
	);

	tag(EntityType::OrdinaryHorse,
		{ 0, 0, 0 },
		{ Summoner, Unenchantable, NoRes }
	);

	tag(EntityType::CauldronBoss,
		{ 0, 0, 0 },
		{ Strong, Miniboss, Special }
	);

	tag(EntityType::SlimeCauldron,
		{ 0, 0, 0 },
		{ Weak, Special, NoRes }
	);

	tag(EntityType::Bat,
		{ 0, 0, 0 },
		{ Floating, Friendly, Pet }
	);

	tag(EntityType::Bee,
		{ 0, 0, 0 },
		{ Floating, NoRes, Pet }
	);

	tag(EntityType::ArchIllager,
		{ 0, 0, 0 },
		{ Miniboss, Special, Illager }
	);

	tag(EntityType::ArchVessel,
		{ 0, 0, 0 },
		{ Miniboss, Special }
	);

	tag(EntityType::ArchVisage,
		{ 0, 0, 0 },
		{ Ranged, NoRes }
	);

	tag(EntityType::JackOLantern,
		{ 0, 0, 0 },
		{ Weak }
	);

	tag(EntityType::RoyalGuard,
		{ 0, 0, 0 },
		{ Strong, Illager }
	);

	tag(EntityType::RoyalGuardAncient,
		{ 0, 0, 0 },
		{ Strong, Illager, Ancient, Special }
	);

	tag(EntityType::VindicatorChef,
		{ 0, 1, 1 },
		{ Strong, Melee, Illager }
	);

	tag(EntityType::Llama,
		{ 0, 1, 1 },
		{ Strong, Friendly, Fast, Pet }
	);

	tag(EntityType::SoulWizard,
		{ 0, 1, 1 },
		{ Strong, Friendly, Fast, Pet }
	);

	tag(EntityType::Enderman,
		{ 1, 1, 1 },
		{ Strong, Miniboss, EventMob, Special, Ender }
	);

	tag(EntityType::Ghast,
		{ 1, 1, 1 },
		{ Strong, Miniboss, EventMob, Special }
	);

	tag(EntityType::ZombieVariant0,
		{ 0, 0, 0 },
		{ Weak }
	);

	tag(EntityType::ZombieVariant1,
		{ 1, 1, 1 },
		{ Weak }
	);

	tag(EntityType::ZombieVariant2,
		{ 1, 1, 1 },
		{ Strong }
	);

	tag(EntityType::ZombieAncient,
		{ 1, 1, 1 },
		{ Strong, Ancient, Special }
	);

	tag(EntityType::SkeletonVariant0,
		{ 0, 0, 0 },
		{ Ranged, Weak }
	);

	tag(EntityType::SkeletonVariant1,
		{ 1, 1, 1 },
		{ Ranged, Weak }
	);

	tag(EntityType::SkeletonVariant2,
		{ 1, 1, 1 },
		{ Ranged, Strong }
	);

	tag(EntityType::PiglinMelee,
		{ 0, 1, 1 },
		{ Strong, Melee }
	);

	tag(EntityType::PiglinMeleeVariant0,
		{ 0, 1, 1 },
		{ Strong, Melee }
	);

	tag(EntityType::PiglinMeleeVariant1,
		{ 0, 1, 1 },
		{ Strong, Melee }
	);

	tag(EntityType::PiglinMeleeVariant2,
		{ 0, 1, 1 },
		{ Strong, Melee }
	);

	tag(EntityType::PiglinRanged,
		{ 0, 1, 1 },
		{ Strong, Ranged }
	);

	tag(EntityType::PiglinRangedVariant0,
		{ 0, 1, 1 },
		{ Strong, Ranged }
	);

	tag(EntityType::PiglinRangedVariant1,
		{ 0, 1, 1 },
		{ Strong, Ranged }
	);

	tag(EntityType::PiglinRangedVariant2,
		{ 0, 1, 1 },
		{ Strong, Ranged }
	);

	tag(EntityType::ZombifiedPiglinMelee,
		{ 0, 1, 1 },
		{ Strong, Melee }
	);

	tag(EntityType::ZombifiedPiglinMeleeVariant0,
		{ 0, 1, 1 },
		{ Strong, Melee }
	);

	tag(EntityType::ZombifiedPiglinMeleeVariant1,
		{ 0, 1, 1 },
		{ Strong, Melee }
	);

	tag(EntityType::ZombifiedPiglinMeleeVariant2,
		{ 0, 1, 1 },
		{ Strong, Melee }
	);

	tag(EntityType::ZombifiedPiglinRanged,
		{ 0, 1, 1 },
		{ Strong, Ranged }
	);

	tag(EntityType::ZombifiedPiglinRangedVariant0,
		{ 0, 1, 1 },
		{ Strong, Ranged }
	);

	tag(EntityType::ZombifiedPiglinRangedVariant1,
		{ 0, 1, 1 },
		{ Strong, Ranged }
	);

	tag(EntityType::ZombifiedPiglinRangedVariant2,
		{ 0, 1, 1 },
		{ Strong, Ranged }
	);

	tag(EntityType::SkeletonAncient,
		{ 1, 1, 1 },
		{ Ranged, Strong, Ancient, Special }
	);

	tag(EntityType::VindicatorVariant0,
		{ 0, 1, 1 },
		{ Strong, Melee, Illager }
	);

	tag(EntityType::VindicatorVariant1,
		{ 1, 1, 1 },
		{ Strong, Melee, Illager }
	);

	tag(EntityType::VindicatorVariant2,
		{ 1, 1, 1 },
		{ Strong, Melee, Illager }
	);

	tag(EntityType::PillagerVariant0,
		{ 0, 0, 0 },
		{ Strong, Ranged, Illager }
	);

	tag(EntityType::PillagerVariant1,
		{ 1, 1, 1 },
		{ Strong, Melee, Illager }
	);

	tag(EntityType::PillagerVariant2,
		{ 1, 1, 1 },
		{ Strong, Ranged, Illager }
	);

	tag(EntityType::PillagerAncient,
		{ 1, 1, 1 },
		{ Strong, Ranged, Illager, Ancient, Special }
	);

	tag(EntityType::ChickenJockeyTower,
		{ 1, 1, 1 },
		{ Strong }
	);

	tag(EntityType::ChickenJockeyTowerAncient,
		{ 1, 1, 1 },
		{ Strong, Ancient, Special }
	);

	tag(EntityType::BabyChicken,
		{ 1, 1, 1 },
		{ Friendly, Pet, Cosmetic }
	);

	tag(EntityType::BabyPig,
		{ 1, 1, 1 },
		{ Friendly, Pet, DLC, Cosmetic }
	);

	tag(EntityType::BabyGhast,
		{ 1, 1, 1 },
		{ Friendly, Pet, DLC, Cosmetic }
	);

	tag(EntityType::JungleZombie,
		{ 0, 0, 0 },
		{ Melee, Weak, DLC }
	); 

	tag(EntityType::MossySkeleton,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC }
	);

	tag(EntityType::QuickGrowingVine,
		{ 0, 0, 0 },
		{ Weak, DLC, Immobile, Unlovable, LowPriority, Unenchantable, NoRes }
	);

	tag(EntityType::QuickGrowingVineSimple,
		{ 0, 0, 0 },
		{ Weak, DLC, Immobile, Unlovable, LowPriority, Unenchantable, NoRes }
	);

	tag(EntityType::QuickGrowingKelp,
		{ 0, 0, 0 },
		{ Weak, DLC, Immobile, Unlovable, LowPriority, Unenchantable, Underwater, NoRes }
	);

	tag(EntityType::PoisonQuillVine,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC, Immobile, NoRes }
	);

	tag(EntityType::PoisonQuillVineSimple,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC, Immobile, Unlovable, NoRes }
	);

	tag(EntityType::PoisonAnemone,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC, Immobile, Unlovable, NoRes, Underwater }
	);

	tag(EntityType::EntangleVine,
		{ 0, 0, 0 },
		{ DLC, Immobile, Unlovable, Unenchantable, NoRes }
	);

	tag(EntityType::AbominationVine,
		{ 0, 0, 0 },
		{ DLC, Immobile, Unlovable, Unenchantable, NoRes }
	);

	tag(EntityType::Ocelot,
		{ 0, 0, 0 },
		{ Weak, Passive, JungleAnimal, BluffAnimal, DLC }
	);

	tag(EntityType::Panda,
		{ 0, 0, 0 },
		{ Weak, Passive, BluffAnimal, DLC }
	);

	tag(EntityType::PlayfulPanda,
		{ 0, 0, 0 },
		{ Weak, Passive, BluffAnimal, DLC }
	);

	tag(EntityType::LazyPanda,
		{ 0, 0, 0 },
		{ Weak, Passive, BluffAnimal, DLC }
	);

	tag(EntityType::BabyPanda,
		{ 0, 0, 0 },
		{ Weak, Passive, BluffAnimal, DLC }
	);

	tag(EntityType::BrownPanda,
		{ 0, 0, 0 },
		{ Weak, Passive, BluffAnimal, DLC }
	);

	tag(EntityType::Leaper,
		{ 0, 0, 0 },
		{ Melee, Strong, DLC, Unlovable }
	);

	tag(EntityType::LeaperAncient,
		{ 0, 0, 0 },
		{ Melee, Strong, DLC, Unlovable, Ancient, Special }
	);

	tag(EntityType::Whisperer,
		{ 0, 0, 0 },
		{ Melee, Weak, DLC }
	);

	tag(EntityType::WaveWhisperer,
		{ 0, 0, 0 },
		{ Melee, Weak, DLC, Underwater }
	);

	tag(EntityType::JungleAbomination,
		{ 0, 0, 0 },
		{ Miniboss, DLC, Special }
	);

	tag(EntityType::Parrot,
		{ 0, 0, 0 },
		{ Weak, Passive, DLC, Floating, Friendly, Pet, Cosmetic }
	);

	tag(EntityType::FrozenZombie,
		{ 0, 0, 0 },
		{ Melee, Ranged, DLC }
	);

	tag(EntityType::IcyCreeper,
		{ 0, 0, 0 },
		{ Melee, Aoe, DLC }
	);

	tag(EntityType::Stray,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC }
	);

	tag(EntityType::PolarBear,
		{ 0, 0, 0 },
		{ Weak, Passive, WinterAnimal, DLC }
	);

	tag(EntityType::ArcticFox,
		{ 0, 0, 0 },
		{ Friendly, DLC, Pet, Cosmetic }
	);

	tag(EntityType::Illusioner,
		{ 1, 1, 1 },
		{ DLC, Strong, Miniboss, EventMob, Unlovable, Special, Illager }
	);

	tag(EntityType::IllusionerClone,
		{ 0, 0, 0 },
		{ DLC, Weak, Unlovable, Special, NoRes }
	);

	tag(EntityType::Chillager,
		{ 0, 0, 0 },
		{ Ranged, Caster, DLC, Illager }
	);

	tag(EntityType::WickedWraith,
		{ 0, 0, 0 },
		{ DLC, Miniboss, Special }

	);
	tag(EntityType::TropicalSlimeLarge,
		{ 0, 1, 1 },
		{ Melee, Strong, DLC, Underwater }
	);

	tag(EntityType::TropicalSlimeMedium,
		{ 0, 1, 1 },
		{ Melee, DLC, Underwater }
	);

	tag(EntityType::TropicalSlimeSmall,
		{ 0, 1, 1 },
		{ Melee, Weak, DLC, Underwater }
	);

	tag(EntityType::Squid,
		{ 0, 0, 0 },
		{ Weak, Passive, DLC, Underwater, OceanAnimal }
	);
	tag(EntityType::GlowSquid,
		{ 0, 0, 0 },
		{ Weak, Passive, DLC, Underwater }
	);
	tag(EntityType::Turtle,
		{ 0, 0, 0 },
		{ Weak, Passive, DLC, Underwater, OceanAnimal }
	);
	tag(EntityType::BabyTurtle,
		{ 0, 0, 0 },
		{ Weak, Passive, DLC, Underwater, OceanAnimal }
	);
	tag(EntityType::Pufferfish,
		{ 0, 0, 0 },
		{ Unenchantable, Weak, Passive, DLC, Underwater, OceanAnimal }
	);
	tag(EntityType::Drowned,
		{ 0, 0, 0 },
		{ Melee, Weak, DLC, Underwater }
	);
	tag(EntityType::DrownedAncient,
		{ 0, 0, 0 },
		{ Melee, Weak, DLC, Underwater, Ancient, Special }
	);

	tag(EntityType::DrownedVariant0,
		{ 0, 0, 0 },
		{ Melee, Weak, DLC, Underwater }
	);

	tag(EntityType::DrownedVariant1,
		{ 0, 0, 0 },
		{ Melee, Weak, DLC, Underwater }
	);

	tag(EntityType::DrownedVariant2,
		{ 0, 0, 0 },
		{ Melee, Weak, DLC, Underwater }
	);

	tag(EntityType::TridentDrowned,
		{ 0, 0, 0 },
		{ Melee, Ranged, Weak, DLC, Underwater }
	);

	tag(EntityType::TridentDrownedVariant0,
		{ 0, 0, 0 },
		{ Melee, Ranged, Weak, DLC, Underwater }
	);

	tag(EntityType::TridentDrownedVariant1,
		{ 0, 0, 0 },
		{ Melee, Ranged, Weak, DLC, Underwater }
	);

	tag(EntityType::TridentDrownedVariant2,
		{ 0, 0, 0 },
		{ Melee, Ranged, Weak, DLC, Underwater }
	);

	tag(EntityType::Guardian,
		{ 0, 0, 0 },
		{ Melee, Ranged, DLC, Underwater }
	);

	tag(EntityType::GuardianAncient,
		{ 0, 0, 0 },
		{ Melee, Ranged, DLC, Underwater, Ancient, Special }
	);

	tag(EntityType::ElderGuardian,
		{ 0, 0, 0 },
		{ Melee, Ranged, EventMob, DLC, Miniboss, Underwater }
	);

	tag(EntityType::Dolphin,
		{ 0, 0, 0 },
		{ Weak, Passive, DLC, Underwater, OceanAnimal }
	);

	tag(EntityType::SunkenSkeleton,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC, Underwater }
	);

	tag(EntityType::SunkenSkeletonVariant0,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC, Underwater }
	);

	tag(EntityType::SunkenSkeletonVariant1,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC, Underwater }
	);

	tag(EntityType::SunkenSkeletonVariant2,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC, Underwater }
	);

	tag(EntityType::DrownedNecromancer,
		{ 0, 1, 1 },
		{ Strong, Summoner, Ranged, Underwater, Miniboss, EventMob }
	);

	tag(EntityType::Blastling,
		{ 0, 0, 0 },
		{ Ranged, Weak, DLC, Ender }
	);

	tag(EntityType::Snareling,
		{ 0, 0, 0 },
		{ Melee, Ranged, DLC, Ender }
	);

	tag(EntityType::SnarelingAncient,
		{ 0, 0, 0 },
		{ Melee, Ranged, DLC, Ender, Ancient, Special }
	);

	tag(EntityType::Endling,
		{ 0, 0, 0 },
		{ Melee, DLC, Ender }
	);

	tag(EntityType::EndlingAncient,
		{ 0, 0, 0 },
		{ Melee, DLC, Ender, Ancient, Special }
	);

	tag(EntityType::Shulker,
		{ 0, 0, 0 },
		{ Ranged, DLC, Immobile }
	);

	tag(EntityType::ShulkerRespawning,
		{ 0, 0, 0 },
		{ Ranged, DLC, Immobile }
	);

	tag(EntityType::Dummy,
		{ 0, 0, 0 },
		{ Immobile, Unenchantable, NoRes }
	);

	tag(EntityType::BabyGoat,
		{ 1, 1, 1 },
		{ Friendly, Pet, DLC, Cosmetic }
	);

	tag(EntityType::Goat,
		{ 0, 0, 0 },
		{ Weak, Passive, DLC, MountainAnimal }
	);
	tag(EntityType::GoatAncient,
		{ 0, 0, 0 },
		{ DLC, Ancient, Special }
	);
	tag(EntityType::Ravager,
		{ 0, 0, 0 },
		{ Melee, Strong, DLC, Unlovable }
	);
	tag(EntityType::Mountaineer,
		{ 0, 0, 0 },
		{ Weak, Melee, DLC, Illager }
	);
	tag(EntityType::MountaineerVariant0,
		{ 1, 1, 1 },
		{ Weak, Melee, DLC, Illager }
	);
	tag(EntityType::MountaineerVariant1,
		{ 1, 1, 1 },
		{ Weak, Melee, DLC, Illager }
	);
	tag(EntityType::MountaineerVariant2,
		{ 1, 1, 1 },
		{ Strong, Melee, DLC, Illager }
	);
	tag(EntityType::WindCaller,
		{ 0, 1, 1 },
		{ Ranged, Caster, DLC, Illager }
	);
	tag(EntityType::SquallGolem,
		{ 0, 1, 1 },
		{ Strong, DLC, NoRes, Unlovable }
	);
	tag(EntityType::LlamaMob,
		{ 0, 1, 1 },
		{ Weak, Passive, DLC, MountainAnimal }
	);
	tag(EntityType::TempestGolem,
		{ 0, 1, 1 },
		{ Strong, Melee, DLC, Miniboss }
	);
	tag(EntityType::RampartCaptain,
		{ 1, 1, 1 },
		{ Strong, Melee, Illager, NoRes, Special, Unlovable }
	);
	tag(EntityType::WoolyCow,
		{ 0, 0, 0 },
		{ Weak, Passive, MountainAnimal }
	);

	tag(EntityType::BabyTurtlePet,
		{ 0, 0, 0 },
		{ Weak, Passive, DLC, Floating, Friendly, Pet, Underwater, Cosmetic }
	);
	tag(EntityType::AncientGuardian,
		{ 1, 1, 1 },
		{ Ranged, Strong, Miniboss, DLC, Special, Underwater }
	);
	tag(EntityType::Biomine,
		{ 0, 1, 1 },
		{ Unenchantable, Underwater }
	);

	tag(EntityType::VindicatorRaidCaptain,
		{ 1, 1, 1 },
		{ Strong, Miniboss, Illager, Special, Melee, RaidCaptain }
	);

	tag(EntityType::PillagerRaidCaptain,
		{ 1, 1, 1 },
		{ Strong, Miniboss, Illager, Special, Ranged, RaidCaptain }
	);

	tag(EntityType::Endersent,
		{ 0, 1, 1 },
		{ DLC, Miniboss, EventMob, Special, Ender, NoHyperMission, Unchainable }
	);
	tag(EntityType::EndersentThornblight,
		{ 0, 1, 1 },
		{ DLC, Miniboss, EventMob, Special, Ender, NoEnchantedName, NoHyperMission, Unchainable }
	);
	tag(EntityType::EndersentVoidstrike,
		{ 0, 1, 1 },
		{ DLC, Miniboss, EventMob, Special, Ender, NoEnchantedName, NoHyperMission, Unchainable }
	);
	tag(EntityType::EndersentWritherot,
		{ 0, 1, 1 },
		{ DLC, Miniboss, EventMob, Special, Ender, NoEnchantedName, NoHyperMission, Unchainable }
	);
	tag(EntityType::EndersentEverfire,
		{ 0, 1, 1 },
		{ DLC, Miniboss, EventMob, Special, Ender, NoEnchantedName, NoHyperMission, Unchainable }
	);
	tag(EntityType::EndersentSurgefiend,
		{ 0, 1, 1 },
		{ DLC, Miniboss, EventMob, Special, Ender, NoEnchantedName, NoHyperMission, Unchainable }
	);
	tag(EntityType::EndersentDeadeye,
		{ 0, 1, 1 },
		{ DLC, Miniboss, EventMob, Special, Ender, NoEnchantedName, NoHyperMission, Unchainable }
	);
	tag(EntityType::EndermiteMinion,
		{ 1, 1, 1 },
		{ Melee, DLC, Ender }
	);
	tag(EntityType::PerfectFormHeart,
		{ 1, 1, 1 },
		{ Miniboss, DLC, Special, Unchainable, Ender }
	);
	tag(EntityType::Friendermite,
		{ 1, 1, 1 },
		{ Friendly, Pet, DLC, Cosmetic }
	);


	tag(EntityType::ZombifiedBabyPig,
		{ 1, 1, 1 },
		{ Friendly, Pet, DLC, Cosmetic }
	);


	// Add some animals here
	for (auto&& type : { EntityType::Cow, EntityType::Sheep } ) {
		tag(type, { 0, 0, 0 }, { Weak, Animal, Passive });
	}
}

const Tags& MobTags::tags(EntityType type) const {
	return item(type).tags;
}

const Tags& MobTags::tags(const std::string& name) const {
	return tags(EntityTypeFromString(name));
}

float MobTags::difficultyThreshold(EntityType type, EGameDifficulty difficulty) const {
	return item(type).difficultyThresholds[enum_cast(difficulty)];
}

float MobTags::difficultyThreshold(const std::string& name, EGameDifficulty difficulty) const {
	return difficultyThreshold(EntityTypeFromString(name), difficulty);
}

const MobTags& MobTags::singleton() {
	static MobTags instance;
	return instance;
}

MobTags& MobTags::tag(EntityType type, std::initializer_list<float> difficultyThresholds, std::initializer_list<std::string> tags ) {
	auto& item = getOrCreate(type);
	for (auto&& tag : tags) {
		item.tags.add(tag);
	}
	auto head = difficultyThresholds.begin();

	if (difficultyThresholds.size() == 1) {
		//EGameDifficulty::Invalid is assumed to always be 0.
		item.difficultyThresholds = { 0.0f, *head, *head, *head };
	} else if (difficultyThresholds.size() == NumberOfDifficulties) {
		//Invalid difficulty threshold is not configured
		item.difficultyThresholds[enum_cast(EGameDifficulty::Invalid)] = 0.0f;
		for (int i = EGameDifficultyFirstIndex; i < enum_cast(EGameDifficulty::ENUM_COUNT); ++i) {
			item.difficultyThresholds[i] = head[i- EGameDifficultyFirstIndex]; //EGameDifficulty::Invalid offsets assignment from config by 1.
		}
	} else {
		DEBUG_FAIL("Need to be either one or tree threshold arguments!");
	}
	return *this;
}

MobTags::Item& MobTags::getOrCreate(EntityType type) {
	auto it = mTags.find(type);
	if (it == end(mTags)) {
		it = mTags.emplace(type, std::make_unique<Item>()).first;
		_onCreated(type, it->second->tags);
	}
	return *it->second;
}

void MobTags::_onCreated(EntityType type, Tags& tags) {
	static const auto getLookupNamesForType = [](EntityType t) {
		static const std::vector<EntityType> lookupSecondaryNames{
			EntityType::PiglinMelee,
			EntityType::PiglinMeleeVariant0,
			EntityType::PiglinMeleeVariant1,
			EntityType::PiglinMeleeVariant2,
			EntityType::ZombifiedPiglinMelee,
			EntityType::ZombifiedPiglinMeleeVariant0,
			EntityType::ZombifiedPiglinMeleeVariant1,
			EntityType::ZombifiedPiglinMeleeVariant2
		};
		return algo::contains(lookupSecondaryNames, t) ? EntityTypeStrings(t) : std::vector<std::string>{ EntityTypeToString(t) };
	};

	for (const auto& name : getLookupNamesForType(type)) {
		tags.add(name);
		tags.add(Util::allRemoved(name, { "_", "-" }));
	}

	if (EntityClassTree::isTypeInstanceOf(type, EntityType::Animal)) {
		tags.add(Animal);
	}
	if (EntityClassTree::isTypeInstanceOf(type, EntityType::Monster)) {
		tags.add(Monster);
	}
	if (EntityClassTree::isTypeInstanceOf(type, EntityType::UndeadMob)) {
		tags.add(Undead);
	}
}

const MobTags::Item& MobTags::item(EntityType type) const {
	auto it = mTags.find(type);

	if (it != end(mTags)) {
		return *it->second;
	}
	return Empty;
}

const MobTags::Item MobTags::Empty{ {}, {0,0,0} };

bool hasMobTag(EntityType type, const std::string& tag) {
	return hasMobTag_alreadyLowerCase(type, Util::toLower(tag));
}

bool hasMobTag(EntityType type, size_t tag_hash) {
	return MobTags::singleton().tags(type).has(tag_hash);
}

bool hasMobTag(const AActor* actor, size_t tag_hash) {
	const auto mob = Cast<AMobCharacter>(actor);
	return mob && hasMobTag(mob->EntityType, tag_hash);
}

bool hasMobTag_alreadyLowerCase(EntityType type, const std::string& lowerCaseTag) {
	return MobTags::singleton().tags(type).has_alreadyLowerCase(lowerCaseTag);
}

bool hasMobTag_alreadyLowerCase(const AActor* actor, const std::string& lowerCaseTag) {
	const auto mob = Cast<AMobCharacter>(actor);
	return mob && hasMobTag_alreadyLowerCase(mob->EntityType, lowerCaseTag);
}

