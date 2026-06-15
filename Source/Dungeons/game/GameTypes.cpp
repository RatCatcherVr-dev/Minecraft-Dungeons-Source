#include "Dungeons.h"
#include "GameTypes.h"

#include "util/AssetUtil.h"
#include "actor/character/mob/MobCharacter.h"
#include "mob/MobTypeDefs.h"
#include "util/Algo.h"

namespace game {

const FString PREFAB_PATH(TEXT("/Game/")); //get from conf
FString PrefabPath(const FString& p) {
	if (!p.Contains("Game/"))
		return PREFAB_PATH + p;
	return p;

}

FString PrefabPath(const char* p) {
	return PrefabPath(FString(p));
}

FString PrefabPath(const FString& p, int suffix) {
	return PrefabPath(p + "_" + FString::FromInt(suffix));

}

const TypeMap& TypeMap::singleton() {
	static TypeMap instance;
	return instance;
}

TypeMap::TypeMap() {
	_register(EntityType::BabyZombie, "Actors/Characters/Enemies/BabyZombie/BP_BabyZombieCharacter");
	_register(EntityType::BabyDrowned, "Content_DLC5/Actors/Characters/Enemies/DrownedBaby/BP_BabyDrownedCharacter");
	_register(EntityType::BabyZombieAncient, "Content_DLC4/Actors/Characters/Enemies/TheTinyScourge/BP_TheTinyScourgeCharacter");
	_register(EntityType::Cow,      "Actors/Characters/Friendlies/Animals/Cow/BP_CowCharacter");
	_register(EntityType::Mooshroom, "Actors/Characters/Enemies/Mooshroom/BP_MooShroomCharacter");
	_register(EntityType::MooshroomAncient, "Content_DLC4/Actors/Characters/Enemies/MooshroomAncient/BP_MooShroomAncientCharacter");
	_register(EntityType::Creeper,  "Actors/Characters/Enemies/Creeper/BP_CreeperBtCharacter");
	_register(EntityType::ChargedCreeper, "Content_DLC4/Actors/Characters/Enemies/ChargedCreeper/BP_ChargedCreeper");
	_register(EntityType::Husk,	    "Actors/Characters/Enemies/Husk/BP_Husk");
	_register(EntityType::Pig,      "Actors/Characters/Friendlies/Animals/Pig/BP_PigCharacter");
	_register(EntityType::Sheep,    "Actors/Characters/Friendlies/Animals/Sheep/BP_SheepBtCharacter");
	_register(EntityType::SheepFireRed, "Patch2/Actors/Characters/Friendlies/Animals/Sheep/BP_SheepFireRed");
	_register(EntityType::SheepPoisonGreen, "Patch2/Actors/Characters/Friendlies/Animals/Sheep/BP_SheepPoisonGreen");
	_register(EntityType::SheepSpeedBlue, "Patch2/Actors/Characters/Friendlies/Animals/Sheep/BP_SheepSpeedBlue");
	_register(EntityType::Wolf,    "Actors/Characters/Friendlies/Animals/Wolf/BP_WolfCharacter");
	_register(EntityType::BabyChicken, "Actors/Characters/Friendlies/Animals/ChickenBaby/BP_BabyChicken");
	_register(EntityType::BabyPig, "Actors/Characters/Friendlies/Animals/BabyPig/BP_BabyPig");
	_register(EntityType::BabyGhast, "Content_DLC4/Actors/Characters/Friendlies/BabyGhast/BP_BabyGhastCharacter");
	_register(EntityType::IronGolem, "Actors/Characters/Friendlies/IronGolem/BP_IronGolemCharacter");
	_registerMobType(EntityType::Skeleton);
	_register(EntityType::SkeletonVariant0, "Actors/Characters/Enemies/Skeleton/BP_SkeletonBtCharacter");
	_register(EntityType::SkeletonVariant1, "Actors/Characters/Enemies/Skeleton/BP_SkeletonVariant1Character");
	_register(EntityType::SkeletonVariant2, "Actors/Characters/Enemies/Skeleton/BP_SkeletonVariant2Character");
	_register(EntityType::SkeletonAncient, "Content_DLC4/Actors/Characters/Enemies/Barrage/BP_BarrageCharacter");
	_register(EntityType::Spider,   "Actors/Characters/Enemies/Spider/BP_SpiderBtCharacter");
	_register(EntityType::SpiderAncient, "Content_DLC4/Actors/Characters/Enemies/AbominableWeaver/BP_AbominableWeaverCharacter");
	_registerMobType(EntityType::Zombie);
	_register(EntityType::ZombieVariant0, "Actors/Characters/Enemies/Zombie/BP_ZombieBtCharacter");
	_register(EntityType::ZombieVariant1, "Actors/Characters/Enemies/Zombie/BP_ZombieVariant1Character");
	_register(EntityType::ZombieVariant2, "Actors/Characters/Enemies/Zombie/BP_ZombieVariant2Character");
	_register(EntityType::ZombieAncient, "Content_DLC4/Actors/Characters/Enemies/GrimGuardian/BP_GrimGuardianCharacter");
	_register(EntityType::Necromancer, "Actors/Characters/Enemies/SkeletonNecromancer/BP_SkeletonNecromancerCharacter");
	_register(EntityType::NecromancerAncient, "Content_DLC4/Actors/Characters/Enemies/HauntedCaller/BP_HauntedCallerCharacter");
	_register(EntityType::RedstoneGolem, "Actors/Characters/Enemies/RedstoneGolem/BP_RedstoneGolemCharacter");
	_register(EntityType::RedstoneGolemAncient, "Content_DLC4/Actors/Characters/Enemies/UnbreakableOne/BP_UnbreakableOneCharacter");
	_register(EntityType::Enchanter, "Actors/Characters/Enemies/Illagers/Enchanter/BP_EnchanterCharacter");
	_register(EntityType::EnchanterAncient, "Content_DLC4/Actors/Characters/Enemies/Illagers/FirstEnchanter/BP_FirstEnchanterCharacter");
	_registerMobType(EntityType::Vindicator);
	_register(EntityType::VindicatorVariant0, "Actors/Characters/Enemies/Illagers/Vindicator/BP_IllagerVindicatorCharacter");
	_register(EntityType::VindicatorVariant1, "Actors/Characters/Enemies/Illagers/Vindicator/BP_IllagerVindicatorVariant1Character");
	_register(EntityType::VindicatorVariant2, "Actors/Characters/Enemies/Illagers/Vindicator/BP_IllagerVindicatorVariant2Character");
	_register(EntityType::Vex,      "Actors/Characters/Enemies/Vex/BP_VexCharacter");
	_register(EntityType::VexAncient, "Content_DLC4/Actors/Characters/Enemies/TheSeekingFlame/BP_TheSeekingFlameCharacter");
	_register(EntityType::Wraith,	"Actors/Characters/Enemies/Wraith/BP_WraithReloaded");
	_register(EntityType::WraithAncient, "Content_DLC4/Actors/Characters/Enemies/AncientTerror/BP_AncientTerrorReloadedCharacter");
	_register(EntityType::Enderman, "Actors/Characters/Enemies/Enderman/BP_EndermanBtCharacter");
	_register(EntityType::Ghast, "Content_DLC4/Actors/Characters/Enemies/Ghast/BP_GhastCharacter");
	_register(EntityType::Blaze, "Content_DLC4/Actors/Characters/Enemies/Blaze/BP_BlazeCharacter");
	_register(EntityType::HoveringInferno, "Content_DLC4/Actors/Characters/Enemies/HoveringInferno/BP_HoveringInfernoCharacter");
	_register(EntityType::BlazeSpawner, "Content_DLC4/Actors/Characters/Enemies/BlazeSpawner/BP_BlazeSpawner");
	_register(EntityType::BlazeSpawnerAncient, "Content_DLC4/Actors/Characters/Enemies/TheUnending/BP_TheUnendingCharacter");
	_register(EntityType::VindicatorRaidCaptain, "Content_DLC5/Actors/Characters/Enemies/RaidCaptain/BP_Vindicator_RaidCaptain");
	_register(EntityType::PillagerRaidCaptain, "Content_DLC5/Actors/Characters/Enemies/RaidCaptain/BP_Pillager_RaidCaptain");

	_register(EntityType::Evoker,   "Actors/Characters/Enemies/Illagers/Evoker/BP_IllagerEvokerCharacter");
	_register(EntityType::EvokerFang, "Actors/Characters/Enemies/Illagers/Evoker/Fang/BP_EvokerFangCharacter");
	_register(EntityType::Geomancer, "Actors/Characters/Enemies/Illagers/Geomancer/BP_IllagerGeomancerCharacter");
	_register(EntityType::GeomancerWall, "Actors/Characters/Enemies/Illagers/Geomancer/GeomancerWall/BP_GeomancerWallCharacter");
	_register(EntityType::GeomancerBomb, "Actors/Characters/Enemies/Illagers/Geomancer/GeomancerBomb/BP_GeomancerBombCharacter");
	_register(EntityType::GeomancerAncient, "Content_DLC4/Actors/Characters/Enemies/Illagers/FrostWarden/BP_FrostWardenCharacter");
	_register(EntityType::ChickenJockey, "Actors/Characters/Enemies/ChickenJockey/BP_ChickenJockeyCharacter");
	_register(EntityType::ChickenJockeyTower, "Actors/Characters/Enemies/ChickenJockeyTower/BP_ChickenJockeyTowerCharacter");
	_register(EntityType::ChickenJockeyTowerAncient, "Content_DLC4/Actors/Characters/Enemies/TheTower/BP_TheTowerCharacter");
	_register(EntityType::SlimeLarge,    "Actors/Characters/Enemies/Slime/SlimeLarge/BP_SlimeLargeCharacter");
	_register(EntityType::SlimeMedium,    "Actors/Characters/Enemies/Slime/SlimeMedium/BP_SlimeMediumCharacter");
	_register(EntityType::SlimeSmall,    "Actors/Characters/Enemies/Slime/SlimeSmall/BP_SlimeSmallCharacter");
	_register(EntityType::SlimeSmallAncient, "Content_DLC4/Actors/Characters/Enemies/OozingMenace/BP_OozingMenaceCharacter");
	_register(EntityType::MagmaCubeLarge, "Content_DLC4/Actors/Characters/Enemies/MagmaCube/BP_MagmaCubeCharacter");
	_register(EntityType::MagmaCubeMedium, "Content_DLC4/Actors/Characters/Enemies/MagmaCubeMedium/BP_MagmaCubeMediumCharacter");
	_register(EntityType::MagmaCubeSmall, "Content_DLC4/Actors/Characters/Enemies/MagmaCubeSmall/BP_MagmaCubeSmallCharacter");
	_register(EntityType::RedstoneMonstrosity, "Actors/Characters/Enemies/RedStoneMonstrosity/BP_RedstoneMonstrosityCharacter");
	_register(EntityType::MooshroomMonstrosity, "Actors/Characters/Enemies/MooshroomMonstrosity/BP_MooshroomMonstrosityCharacter");
	_register(EntityType::RedstoneCube, "Actors/Characters/Enemies/RedStoneCube/BP_RedstoneCubeCharacter");
	_register(EntityType::NamelessKing, "Actors/Characters/Enemies/NamelessKing/BP_NamelessKingCharacter");
	_register(EntityType::Witch, "Actors/Characters/Enemies/Witch/BP_WitchCharacter");
	_register(EntityType::WitchAncient, "Content_DLC4/Actors/Characters/Enemies/PestilentConjurer/BP_PestilentConjurerCharacter");
	_register(EntityType::CaveSpider, "Actors/Characters/Enemies/CaveSpider/BP_CaveSpiderCharacter");
	_registerMobType(EntityType::Pillager);
	_register(EntityType::PillagerVariant0, "Actors/Characters/Enemies/Illagers/Pillager/BP_PillagerCharacter");
	_register(EntityType::PillagerVariant1, "Actors/Characters/Enemies/Illagers/Pillager/BP_PillagerVariant1Character");
	_register(EntityType::PillagerVariant2, "Actors/Characters/Enemies/Illagers/Pillager/BP_PillagerVariant2Character");
	_register(EntityType::PillagerAncient, "Content_DLC4/Actors/Characters/Enemies/Illagers/SolemnGiant/BP_SolemnGiantCharacter");
	_register(EntityType::FalseKing, "Actors/Characters/Enemies/FalseKing/BP_FalseKingCharacter");
	_register(EntityType::SkeletonVanguard, "Actors/Characters/Enemies/SkeletonVanguard/BP_SkeletonVanguardCharacter");
	_register(EntityType::SkeletonVanguardAncient, "Content_DLC4/Actors/Characters/Enemies/CursedPresence/BP_CursedPresenceCharacter");
	_register(EntityType::MobSpawner, "Actors/Characters/Enemies/MobSpawner/BP_MobSpawnerCharacter");
	_register(EntityType::Rabbit, "Actors/Characters/Friendlies/Animals/Rabbit/BP_RabbitCharacter");
	_register(EntityType::Hoglin, "Content_DLC4/Actors/Characters/Enemies/Hoglin/BP_HoglinCharacter");
	_register(EntityType::HoglinAncient, "Content_DLC4/Actors/Characters/Enemies/UnstoppableTusk/BP_UnstoppableTuskCharacter");
	_register(EntityType::PiglinFungusThrower, "Content_DLC4/Actors/Characters/Enemies/PiglinFungusThrower/BP_PiglinFungusThrowerCharacter");
	_register(EntityType::ZombifiedPiglinFungusThrower, "Content_DLC4/Actors/Characters/Enemies/PiglinFungusThrower/BP_ZombifiedPiglinFungusThrower");
	_registerMobType(EntityType::PiglinMelee);
	_register(EntityType::PiglinMeleeVariant0, "Content_DLC4/Actors/Characters/Enemies/Piglin/BP_PiglinMeleeVariant0Character");
	_register(EntityType::PiglinMeleeVariant1, "Content_DLC4/Actors/Characters/Enemies/Piglin/BP_PiglinMeleeVariant1Character");
	_register(EntityType::PiglinMeleeVariant2, "Content_DLC4/Actors/Characters/Enemies/Piglin/BP_PiglinMeleeVariant2Character");
	_registerMobType(EntityType::PiglinRanged);
	_register(EntityType::PiglinRangedVariant0, "Content_DLC4/Actors/Characters/Enemies/Piglin/BP_PiglinRangedVariant0Character");
	_register(EntityType::PiglinRangedVariant1, "Content_DLC4/Actors/Characters/Enemies/Piglin/BP_PiglinRangedVariant1Character");
	_register(EntityType::PiglinRangedVariant2, "Content_DLC4/Actors/Characters/Enemies/Piglin/BP_PiglinRangedVariant2Character");
	_registerMobType(EntityType::ZombifiedPiglinMelee);
	_register(EntityType::ZombifiedPiglinMeleeVariant0, "Content_DLC4/Actors/Characters/Enemies/ZombifiedPiglin/BP_ZombifiedPiglinMeleeVariant0Character");
	_register(EntityType::ZombifiedPiglinMeleeVariant1, "Content_DLC4/Actors/Characters/Enemies/ZombifiedPiglin/BP_ZombifiedPiglinMeleeVariant1Character");
	_register(EntityType::ZombifiedPiglinMeleeVariant2, "Content_DLC4/Actors/Characters/Enemies/ZombifiedPiglin/BP_ZombifiedPiglinMeleeVariant2Character");
	_registerMobType(EntityType::ZombifiedPiglinRanged);
	_register(EntityType::ZombifiedPiglinRangedVariant0, "Content_DLC4/Actors/Characters/Enemies/ZombifiedPiglin/BP_ZombifiedPiglinRangedVariant0Character");
	_register(EntityType::ZombifiedPiglinRangedVariant1, "Content_DLC4/Actors/Characters/Enemies/ZombifiedPiglin/BP_ZombifiedPiglinRangedVariant1Character");
	_register(EntityType::ZombifiedPiglinRangedVariant2, "Content_DLC4/Actors/Characters/Enemies/ZombifiedPiglin/BP_ZombifiedPiglinRangedVariant2Character");
	_register(EntityType::SkeletonHorseman, "Actors/Characters/Enemies/SkeletonHorseman/BP_SkeletonHorsemanCharacter");
	_register(EntityType::OrdinaryHorse, "Actors/Characters/Enemies/OrdinaryHorse/BP_OrdinaryHorseCharacter");
	_register(EntityType::CauldronBoss, "Actors/Characters/Enemies/CauldronBoss/BP_CauldronBossCharacter");
	_register(EntityType::SlimeCauldron, "Actors/Characters/Enemies/SlimeCauldron/BP_SlimeCauldronCharacter");
	_register(EntityType::Bat, "Actors/Characters/Enemies/Bat/BP_BatCharacter");
	_register(EntityType::Bee, "Patch2/Actors/Characters/Friendlies/Animals/Bee/BP_BeeCharacter");
	_register(EntityType::ArchIllager, "Actors/Characters/Enemies/Illagers/ArchIllager/BP_ArchIllagerCharacter");
	_register(EntityType::ArchVessel, "Actors/Characters/Enemies/Illagers/ArchVessel/BP_ArchVesselCharacter");
	_register(EntityType::ArchVisage, "Actors/Characters/Enemies/Illagers/ArchVisage/BP_ArchVisageCharacter");
	_register(EntityType::JackOLantern, "Actors/Characters/Enemies/JackOLantern/BP_JackOLanternCharacter");

	// D11.DB - D11 Mobs
	_register(EntityType::JungleZombie, "Actors/Characters/Enemies/JungleZombie/BP_JungleZombieCharacter");
	_register(EntityType::MossySkeleton, "Actors/Characters/Enemies/MossySkeleton/BP_MossySkeletonCharacter");
	_register(EntityType::QuickGrowingVine, "Actors/Characters/Enemies/QuickGrowingVine/BP_QuickGrowingVine");
	_register(EntityType::QuickGrowingVineSimple, "Actors/Characters/Enemies/QuickGrowingVine/BP_QuickGrowingVineSimple");
	_register(EntityType::QuickGrowingKelp, "Content_DLC5/Actors/Characters/Enemies/QuickGrowingKelp/BP_QuickGrowingKelp");
	_register(EntityType::EntangleVine, "Actors/Characters/Enemies/EntangleVine/BP_EntangleVineCharacter");
	_register(EntityType::AbominationVine, "Actors/Characters/Enemies/JungleAbominationVine/BP_AbominationVineCharacter");
	_register(EntityType::PoisonQuillVine, "Actors/Characters/Enemies/PoisonQuillVine/BP_PoisonQuillVine");
	_register(EntityType::PoisonQuillVineSimple, "Actors/Characters/Enemies/PoisonQuillVine/BP_PoisonQuillVine");
	_register(EntityType::PoisonAnemone, "Content_DLC5/Actors/Characters/Enemies/PoisonAnemone/BP_PoisonAnemone");
    _register(EntityType::Ocelot, "Actors/Characters/Friendlies/Animals/Ocelot/BP_OcelotCharacter");
	_registerMobType(EntityType::Panda);
	_register(EntityType::PlayfulPanda, "Actors/Characters/Friendlies/Animals/Panda/BP_PlayfulPandaCharacter");
	_register(EntityType::LazyPanda, "Actors/Characters/Friendlies/Animals/Panda/BP_LazyPandaCharacter");
	_register(EntityType::BabyPanda, "Actors/Characters/Friendlies/Animals/BabyPanda/BP_BabyPandaCharacter");
	_register(EntityType::BrownPanda, "Actors/Characters/Friendlies/Animals/Panda/BP_BrownPandaCharacter");
	_register(EntityType::Leaper, "Actors/Characters/Enemies/Leaper/BP_LeaperCharacter");
	_register(EntityType::LeaperAncient, "Content_DLC4/Actors/Characters/Enemies/ThunderingGrowth/BP_ThunderingGrowthCharacter");
	_register(EntityType::Whisperer, "Actors/Characters/Enemies/Whisperer/BP_WhispererCharacter");
	_register(EntityType::WaveWhisperer, "Content_DLC5/Actors/Characters/Enemies/WaveWhisperer/BP_WaveWhispererCharacter");
    _register(EntityType::JungleAbomination, "Actors/Characters/Enemies/JungleAbomination/BP_JungleAbominationCharacter");
    _register(EntityType::Parrot, "Actors/Characters/Friendlies/Animals/Parrot/BP_ParrotCharacter");

	_register(EntityType::IcyCreeper,  "Actors/Characters/Enemies/IcyCreeper/BP_IcyCreeperCharacter");
	_register(EntityType::FrozenZombie, "Actors/Characters/Enemies/FrozenZombie/BP_FrozenZombieCharacter");
	_register(EntityType::Stray, "Actors/Characters/Enemies/Stray/BP_StrayBtCharacter");
	_register(EntityType::PolarBear, "Actors/Characters/Friendlies/Animals/PolarBear/BP_PolarBearBtCharacter");
	_register(EntityType::ArcticFox, "Actors/Characters/Friendlies/Animals/SnowFox/BP_SnowFox");
	_register(EntityType::Illusioner, "Actors/Characters/Enemies/Illusioner/BP_IllusionerCharacter");
	_register(EntityType::IllusionerClone, "Actors/Characters/Enemies/Illusioner/BP_IllusionerCloneCharacter");
	_register(EntityType::Chillager, "Actors/Characters/Enemies/Illagers/Chillager/BP_ChillagerCharacter");
	_register(EntityType::WickedWraith, "Actors/Characters/Enemies/Wicked_Wraith/BP_WickedWraithCharacter");

	_register(EntityType::BabyGoat, "Content_DLC3/Actors/Characters/Friendlies/Animals/GoatBaby/BP_BabyGoatCharacter");
	_register(EntityType::Goat, "Content_DLC3/Actors/Characters/Friendlies/Animals/Goat/BP_GoatCharacter");
	_register(EntityType::GoatAncient, "Content_DLC4/Actors/Characters/Enemies/Windbeard/BP_WindbeardCharacter");
	_register(EntityType::Ravager, "Content_DLC3/Actors/Characters/Enemies/Ravager/BP_RavagerCharacter");
	_registerMobType(EntityType::Mountaineer);
	_register(EntityType::MountaineerVariant0, "Content_DLC3/Actors/Characters/Enemies/Illagers/Mountaineer/BP_MountaineerCharacter");
	_register(EntityType::MountaineerVariant1, "Content_DLC3/Actors/Characters/Enemies/Illagers/Mountaineer/BP_MountaineerVariant1Character");
	_register(EntityType::MountaineerVariant2, "Content_DLC3/Actors/Characters/Enemies/Illagers/Mountaineer/BP_MountaineerVariant2Character");
	_register(EntityType::WindCaller, "Content_DLC3/Actors/Characters/Enemies/Windcaller/BP_WindcallerCharacter");
	_register(EntityType::SquallGolem, "Content_DLC3/Actors/Characters/Enemies/Squall_Golem/BP_SquallGolemCharacter");
	_register(EntityType::TempestGolem, "Content_DLC3/Actors/Characters/Enemies/Tempest_Golem/BP_TempestGolemCharacter");
	_register(EntityType::WoolyCow, "Content_DLC3/Actors/Characters/Friendlies/Animals/WoolyCow/BP_WoolyCowCharacter");
	_register(EntityType::TropicalSlimeLarge, "Content_DLC5/Actors/Characters/Enemies/TropicalSlime/TropicalSlimeLarge/BP_TropicalSlimeLargeCharacter");
	_register(EntityType::TropicalSlimeMedium, "Content_DLC5/Actors/Characters/Enemies/TropicalSlime/TropicalSlimeMedium/BP_TropicalSlimeMediumCharacter");
	_register(EntityType::TropicalSlimeSmall, "Content_DLC5/Actors/Characters/Enemies/TropicalSlime/TropicalSlimeSmall/BP_TropicalSlimeSmallCharacter");
	_register(EntityType::Squid, "Content_DLC5/Actors/Characters/Friendlies/Animals/Squid/BP_SquidCharacter");
	_register(EntityType::Turtle, "Content_DLC5/Actors/Characters/Friendlies/Animals/Turtle/BP_TurtleCharacter");
	_register(EntityType::BabyTurtle, "Content_DLC5/Actors/Characters/Friendlies/Animals/BabyTurtle/BP_BabyTurtleCharacter");
	_register(EntityType::Pufferfish, "Content_DLC5/Actors/Characters/Friendlies/Animals/Pufferfish/BP_PufferfishCharacter");
	_register(EntityType::DrownedAncient, "Content_DLC6/Actors/Characters/Enemies/VengefulMariner/BP_VengefulMarinerCharacter");
	_registerMobType(EntityType::Drowned);
	_register(EntityType::DrownedVariant0, "Content_DLC5/Actors/Characters/Enemies/Drowned/BP_DrownedCharacter");
	_register(EntityType::DrownedVariant1, "Content_DLC5/Actors/Characters/Enemies/Drowned/BP_DrownedVariant1Character");
	_register(EntityType::DrownedVariant2, "Content_DLC5/Actors/Characters/Enemies/Drowned/BP_DrownedVariant2Character");
	_registerMobType(EntityType::TridentDrowned);
	_register(EntityType::TridentDrownedVariant0, "Content_DLC5/Actors/Characters/Enemies/Drowned/BP_TridentDrownedCharacter");
	_register(EntityType::TridentDrownedVariant1, "Content_DLC5/Actors/Characters/Enemies/Drowned/BP_TridentDrownedVariant1Character");
	_register(EntityType::TridentDrownedVariant2, "Content_DLC5/Actors/Characters/Enemies/Drowned/BP_TridentDrownedVariant2Character");
	_register(EntityType::Guardian, "Content_DLC5/Actors/Characters/Enemies/Guardian/BP_GuardianCharacter");
	// This is an ancient variant of the normal guardian mob, NOT to be confused with AncientGuardian defined below
	_register(EntityType::GuardianAncient, "Content_DLC6/Actors/Characters/Enemies/AbyssalEye/BP_AbyssalEyeCharacter");
	_register(EntityType::ElderGuardian, "Content_DLC5/Actors/Characters/Enemies/ElderGuardian/BP_ElderGuardianCharacter");
	_register(EntityType::Dolphin, "Content_DLC5/Actors/Characters/Friendlies/Animals/Dolphin/BP_DolphinCharacter");
	_registerMobType(EntityType::SunkenSkeleton);
	_register(EntityType::SunkenSkeletonVariant0, "Content_DLC5/Actors/Characters/Enemies/SunkenSkeleton/BP_SunkenSkeletonCharacter");
	_register(EntityType::SunkenSkeletonVariant1, "Content_DLC5/Actors/Characters/Enemies/SunkenSkeleton/BP_SunkenSkeletonVariant1Character");
	_register(EntityType::SunkenSkeletonVariant2, "Content_DLC5/Actors/Characters/Enemies/SunkenSkeleton/BP_SunkenSkeletonVariant2Character");
	_register(EntityType::DrownedNecromancer, "Content_DLC5/Actors/Characters/Enemies/DrownedNecromancer/BP_DrownedNecromancerCharacter");
	_register(EntityType::BabyTurtlePet, "Content_DLC5/Actors/Characters/Friendlies/Animals/BabyTurtle/BP_BabyTurtlePetCharacter");
	_register(EntityType::GlowSquid, "Content_DLC5/Actors/Characters/Friendlies/Animals/GlowSquid/BP_GlowSquidCharacter");
	_register(EntityType::AncientGuardian, "Content_DLC5/Actors/Characters/Enemies/Ancient_Guardian/BP_AncientGuardianCharacter");
	_register(EntityType::Biomine, "Content_DLC5/Actors/Characters/Enemies/Ancient_Guardian/Mine/BP_BiomineCharacter");
	// D11.DB - END

	//DLC6
	_register(EntityType::Blastling, "Content_DLC6/Actors/Characters/Enemies/Blastling/BP_BlastlingCharacter");
	_register(EntityType::Snareling, "Content_DLC6/Actors/Characters/Enemies/Snareling/BP_SnarelingCharacter");
	_register(EntityType::SnarelingAncient, "Content_DLC6/Actors/Characters/Enemies/TheSwarm/BP_TheSwarmCharacter");
	_register(EntityType::Endling, "Content_DLC6/Actors/Characters/Enemies/Endling/BP_EndlingCharacter");
	_register(EntityType::EndlingAncient, "Content_DLC6/Actors/Characters/Enemies/WatcherOfTheEnd/BP_WatcherOfTheEndCharacter");
	_register(EntityType::Shulker, "Content_DLC6/Actors/Characters/Enemies/Shulker/BP_ShulkerCharacter");
	_register(EntityType::ShulkerRespawning, "Content_DLC6/Actors/Characters/Enemies/Shulker/BP_ShulkerRespawningCharacter");
	_register(EntityType::Endermite, "Content_DLC6/Actors/Characters/Enemies/Endermite/BP_EndermiteCharacter");
	_register(EntityType::EndermiteSmart, "Content_DLC6/Actors/Characters/Enemies/Endermite/BP_EndermiteCharacter");
	_register(EntityType::Silverfish, "Content_DLC6/Actors/Characters/Enemies/Silverfish/BP_SilverfishCharacter");
	_register(EntityType::SilverfishSmart, "Content_DLC6/Actors/Characters/Enemies/Silverfish/BP_SilverfishCharacter");
	_register(EntityType::SilverfishAncient, "Content_DLC6/Actors/Characters/Enemies/ScuttlingTorment/BP_ScuttlingTormentCharacter");
	_register(EntityType::Endersent, "Content_DLC6/Actors/Characters/Enemies/Endersent/BP_EndersentCharacter");
	_register(EntityType::EndersentThornblight, "Content_DLC6/Actors/Characters/Enemies/Endersent/EnchantedEndersent/BP_EndersentThornblightCharacter");
	_register(EntityType::EndersentVoidstrike, "Content_DLC6/Actors/Characters/Enemies/Endersent/EnchantedEndersent/BP_EndersentVoidstrikeCharacter");
	_register(EntityType::EndersentWritherot, "Content_DLC6/Actors/Characters/Enemies/Endersent/EnchantedEndersent/BP_EndersentWritherotCharacter");
	_register(EntityType::EndersentEverfire, "Content_DLC6/Actors/Characters/Enemies/Endersent/EnchantedEndersent/BP_EndersentEverfireCharacter");
	_register(EntityType::EndersentSurgefiend, "Content_DLC6/Actors/Characters/Enemies/Endersent/EnchantedEndersent/BP_EndersentSurgefiendCharacter");
	_register(EntityType::EndersentDeadeye, "Content_DLC6/Actors/Characters/Enemies/Endersent/EnchantedEndersent/BP_EndersentDeadeyeCharacter");
	_register(EntityType::Seravex, "Content_DLC6/Actors/Characters/Friendlies/Seravex/BP_SeravexCharacter");
	_register(EntityType::EndermiteMinion, "Content_DLC6/Actors/Characters/Enemies/Endermite/BP_EndermiteMinionCharacter");
	_register(EntityType::PerfectFormHeart, "Actors/Characters/Enemies/PerfectForm/BP_PerfectFormCharacter");
	_register(EntityType::Friendermite, "Content_DLC6/Actors/Characters/Friendlies/Animals/Friendermite/BP_FriendermiteCharacter");
	//DLC6 - END

	// SPOOKY FALL 2
	_register(EntityType::ZombifiedBabyPig, "Actors/Characters/Friendlies/Pets/ZombifiedBabyPig/BP_ZombifiedBabyPig");
	// SPOOKY FALL 2 - END



	_register(EntityType::PiggyBank, "Actors/Characters/Friendlies/Animals/PiggyBank/BP_PiggyBankCharacter");
	_register(EntityType::GoldBabyKey, "Actors/Characters/Friendlies/BabyKey/GoldBabyKey/BP_GoldBabyKeyCharacter");
	_register(EntityType::SilverBabyKey, "Actors/Characters/Friendlies/BabyKey/SilverBabyKey/BP_SilverBabyKeyCharacter");

	_register(EntityType::RoyalGuard, "Actors/Characters/Enemies/Illagers/RoyalGuard/BP_IllagerRoyalGuardCharacter");
	_register(EntityType::RoyalGuardAncient, "Content_DLC4/Actors/Characters/Enemies/Illagers/VigilantScoundrel/BP_VigilantScoundrelCharacter");
	_register(EntityType::VindicatorChef, "Actors/Characters/Enemies/Illagers/Vindicator/BP_IllagerVindicatorChefCharacter");
	_register(EntityType::WitherSkeletonRanged, "Content_DLC4/Actors/Characters/Enemies/WitherSkeletonRanged/BP_WitherSkeletonRangedCharacter");
	_register(EntityType::WitherSkeleton, "Content_DLC4/Actors/Characters/Enemies/WitherSkeleton/BP_WitherSkeletonCharacter");
	_register(EntityType::Llama, "Actors/Characters/Friendlies/Animals/Llama/BP_LlamaCharacter");
	_register(EntityType::SoulWizard, "Patch2/Actors/Characters/Friendlies/SoulWizard/BP_SoulWizardCharacter");
	_register(EntityType::LlamaMob, "Actors/Characters/Friendlies/Animals/Llama/BP_LlamaVariant1Character");
	_register(EntityType::RampartCaptain, "Content_DLC3/Actors/Characters/Enemies/Illagers/RampartCaptain/BP_RampartCaptainCharacter");	
}

void TypeMap::_registerMobType(EntityType type) {
	mMobTypes.push_back(type);
}

UClass* TypeMap::mobClass(EntityType type) const {
	if (auto cls = mTypeMap.Find(type)) {
		return cls->getOrLoad();
	}
	return nullptr;
}

const std::vector<EntityType>& TypeMap::mobTypes() const {
	return mMobTypes;
}

void TypeMap::_register(EntityType type, const FString& path) {
	const auto mobTypeDef = mob::type::get(type);
	if (!mobTypeDef.IsDisabled()) {
		check(!mTypeMap.Contains(type) && "Already registered");
		
		mTypeMap.Add(type, LazyClass("/Game/" + path));
		mMobTypes.push_back(type);
		mPathLookup.Add(type, MakeAssetPathFromPackageName("/Game/" + path));
	}
}
//
// Lazy class loading
//
TypeMap::LazyClass::LazyClass(FString path)
	: mPath(path) {}

UClass* TypeMap::LazyClass::getOrLoad() {
	if(!mClass.IsValid()) {
		mClass = ConstructorHelpersInternal::FindOrLoadClass(mPath, AMobCharacter::StaticClass());
		if(!mClass.IsValid()) {
			UE_LOG(LogTemp, Error, TEXT("Couldn't load class: %s"), *mPath);
		}
	}
	
	return mClass.Get();
}

std::vector<FSoftObjectPath> TypeMap::Entities() const {
	std::vector<FSoftObjectPath> vectorInstance;

	return algo::map_vector(mPathLookup, RETLAMBDA(it.Value));
}

FSoftObjectPath TypeMap::PathForEntity(EntityType type) const {
	if (auto* path = mPathLookup.Find(type)) {
		return *path;
	}
	return {};
}

bool TypeMap::IsMapped(EntityType type) const {
	return mPathLookup.Contains(type);
}

}
