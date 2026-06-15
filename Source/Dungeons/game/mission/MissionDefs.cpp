#include "Dungeons.h"
#include "MissionDefs.h"
#include "MutableMissionDef.h"
#include "variation/LevelVariationDefs.h"
#include "variation/LevelVariationDef.h"
#include "theme/MissionThemeDefs.h"
#include "CommonTypes.h"
#include "hyper/HyperMissions.h"
#include "game/GameSettings.h"
#include "game/LevelSettings.h"
#include "game/affector/Affectors.h"
#include "game/difficulty/Difficulty.h"
#include "game/difficulty/ExtraChallenge.h"
#include "game/dlc/DLCDefs.h"
#include "game/item/generator/ItemGeneratorTypes.h"
#include "game/item/generator/ItemGeneratorConfigs.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/power/ItemPowerUtil.h"
#include "game/item/power/ItemPowerStats.h"
#include "game/mission/MissionData.h"
#include "locale/LocTableFromFile.h"
#include "lovika/world/level/levelgen/metascore/Picks.h"
#include "lovika/world/level/levelgen/metascore/Scorers.h"
#include "lovika/world/level/postprocess/PostProcessConfigs.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "util/Math.h"
#include "game/util/ValueFormat.h"
#include "StringTableCore.h"
#include <StringTableRegistry.h>
#include "game/mobspawn/event/EventMobSpawner.h"
#include "EndVideoDefinitions.h"
#include <limits>
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/EquipmentComponent.h"
#include "game/Enchantments/generator/EnchantmentGeneratorUtil.h"
#include "game/component/blocktriggers/SlippyBlockTrigger.h"
#include "game/component/blocktriggers/MagmaBlockTrigger.h"
#include "game/component/blocktriggers/VoidBlockTrigger.h"
#include "game/component/blocktriggers/VoidLiquidBlockTrigger.h"
#include "game/affector/AffectorData.h"
#include "game/merchant/MerchantDefs.h"

namespace missions {

using namespace levelgen::score;

auto missionDefs = Util::createDefaultedTArrayOfSize<Unique<MutableMissionDef>>(enum_cast(ELevelNames::count));
std::unordered_map<ELevelNames, Unique<MissionDef>> missionDefOverrides;

//D11.PS had to change this to default to none
MutableMissionDef& create(ELevelNames levelIndex, ELevelVariationType levelVariation = ELevelVariationType::none) {
	const int index = enum_cast(levelIndex);
	check(!missionDefs[index]);
	missionDefs[index] = make_unique<MutableMissionDef>(levelIndex);
	missionDefs[index]->levelVariation(levelVariation, {});
	missionDefs[index]->setTheme(EMissionTheme::Story);
	return *missionDefs[index];
}

MutableMissionDef& createCopyOf(ELevelNames levelIndex, ELevelNames copyOfIndex) {
	const int index = enum_cast(levelIndex);
	check(!missionDefs[index]);

	const auto& copyOf = missionDefs[enum_cast(copyOfIndex)];
	check(copyOf);

	missionDefs[index] = make_unique<MutableMissionDef>(*copyOf);
	missionDefs[index]->setLevel(levelIndex);
	return *missionDefs[index];
}

#define LOCTEXT_NAMESPACE "Mission"

static const FSoftObjectPath DailyFallback = FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingGame.loadingGame");
static const FSoftObjectPath SecretFallback = FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingGame.loadingGame");

EMissionTheme getDefaultLevelVariationTheme(ELevelVariationType variation) {
	switch (variation) {
	case ELevelVariationType::daily:
		return EMissionTheme::Daily;
	case ELevelVariationType::weekly:
		return EMissionTheme::Weekly;
	case ELevelVariationType::seasonal:
		return EMissionTheme::Seasonal;
	case ELevelVariationType::none:
	case ELevelVariationType::Invalid:
	default:
		check(false && "unexpected, trials without variation don't have a default theme");
		return EMissionTheme::Invalid;
	}
}

MutableMissionDef& createTrial(ELevelNames levelIndex, ELevelNames basedOnLevelIndex, ELevelVariationType levelVariationType) {
	const auto* basedOnDef = getChecked(basedOnLevelIndex);

	return createCopyOf(levelIndex, basedOnLevelIndex)
		.setTheme(getDefaultLevelVariationTheme(levelVariationType))
		.levelVariation(levelVariationType, basedOnLevelIndex)
		.loadingScreen(basedOnDef->getLoadingScreenTexturePath().Get(DailyFallback))
		.addRequiredMission(basedOnLevelIndex)
		.setExtraChallenge(EExtraChallenge::Challenge_3)
		.setNeedsSpecificUnlock(basedOnDef->isSecretMission(), basedOnDef->isSecretMission() ? basedOnLevelIndex : TOptional<ELevelNames>{})
		;
}

MutableMissionDef& createSecret(ELevelNames levelIndex, ELevelNames basedOnLevelIndex) {
	const auto& basedOnDef = get(basedOnLevelIndex);

	return create(levelIndex, ELevelVariationType::none)
		.setTheme(EMissionTheme::Secret)
		.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingscreen_subdungeon.loadingscreen_subdungeon"))
		.name(basedOnDef.getNameText())
		.mapColor(basedOnDef.getMapColors())
		.selectOrder(basedOnDef.getSelectOrder() + 2)		
		.specificUnlockFoundInMission(basedOnLevelIndex)
		.setNeedsSpecificUnlock(true)
		;
}

FMapColors GenerateColorOffsetsFromBase(FLinearColor BaseColor, FLinearColor AboveOffset = FMapColors::DefaultAboveOffset, FLinearColor BelowOffset = FMapColors::DefaultBelowOffset) {
	return { BaseColor, BaseColor + AboveOffset, BaseColor - BelowOffset };
}

namespace em {

// event mobs
struct MutableEventMobDef : EventMobDef {
	MutableEventMobDef(EntityType et) : EventMobDef(et) {}
	MutableEventMobDef& d2() { mMinDifficulty = EGameDifficulty::Difficulty_2; return *this; }
	MutableEventMobDef& d3() { mMinDifficulty = EGameDifficulty::Difficulty_3; return *this; }
	MutableEventMobDef& p(float probability) { mProbability = probability; return *this; }
};

MutableEventMobDef SkeletonHorseman() { return {EntityType::OrdinaryHorse}; }
MutableEventMobDef Enderman() { return { EntityType::Enderman }; }
MutableEventMobDef Endersent() { return { EntityType::Endersent }; }
MutableEventMobDef RedstoneGolem() { return { EntityType::RedstoneGolem }; }
MutableEventMobDef Evoker() { return { EntityType::Evoker }; }
MutableEventMobDef Illusioner() { return { EntityType::Illusioner }; }
MutableEventMobDef Ghast() { return { EntityType::Ghast }; }
MutableEventMobDef ElderGuardian() { return { EntityType::ElderGuardian }; }
MutableEventMobDef DrownedNecromancer() { return { EntityType::DrownedNecromancer}; }


TArray<EventMobDef> AddRemainingApocalypseMobs(TArray<EventMobDef> mobs) {
	TSet<EntityType> remaining { EntityType::OrdinaryHorse, EntityType::Enderman, EntityType::RedstoneGolem, EntityType::Evoker };
	for (const auto& mob : mobs) {
		remaining.Remove(mob.getEntityType());
	}
	for (auto type : remaining) {
		mobs.Add(MutableEventMobDef(type).d3());
	}
	return mobs;
}

}


//
// Mission definitions
//


const MissionDef& Invalid = create(ELevelNames::Invalid, ELevelVariationType::Invalid)
	.permanentlyDisabled()
	;

const MissionDef& CreeperWoods = create(ELevelNames::creeperwoods)
	.name(LOCTEXT("creeperwoods_name", "Creeper Woods"))
	.storyTitle(LOCTEXT("creeperwoods_title", "A Journey Begins..."))
	.storyContents(LOCTEXT("creeperwoods_contents", "The Arch-Illager has come to power using the Orb of Dominance, turning the Overworld upside down. Power is collecting in strange places, the undead are stronger and refuse to burn in the sunlight, and Illagers are raiding the countryside! \n\nThe Illagers capture the defenseless Villagers and move them east, towards their mysterious base in the Redstone Mines. The first step in their ruthless journey starts by going through Creeper Woods, a mob infested maze of tangled roots and branches. Move quickly if you wish to save the Villagers from a terrible fate!"))
	.loadLocstringsFromFile("creeperwoods", [] {LOCTABLE_FROMFILE_GAME("creeperwoods", "creeperwoodsLabels", "Decor/Text/creeperwoodsLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::Overworld()))
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::SkeletonHorseman().d2(), em::Enderman().d2(), em::Evoker().d2() }))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingCreeperWoods.loadingCreeperWoods"))
	.selectOrder(100)
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	;

const MissionDef& CreeperWoodsDaily = createTrial(ELevelNames::creeperwoodsdaily, ELevelNames::creeperwoods, ELevelVariationType::daily);
const MissionDef& CreeperWoodsWeekly = createTrial(ELevelNames::creeperwoodsweekly, ELevelNames::creeperwoods, ELevelVariationType::weekly);
const MissionDef& CreeperWoodsSeasonal = createTrial(ELevelNames::creeperwoodsseasonal, ELevelNames::creeperwoods, ELevelVariationType::seasonal);

const MissionDef& CreepyCrypt = createSecret(ELevelNames::creepycrypt, ELevelNames::creeperwoods)
	.name(LOCTEXT("creeperwoodsbonus_name", "Creepy Crypt"))
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.storyTitle(LOCTEXT("creeperwoodsbonus_title", "The Forsaken Undercroft"))
	.storyContents(LOCTEXT("creeperwoodsbonus_contents", "A sprawling crypt lies deep below the forest floor where the Illagers believe they can continue their wicked work undisturbed. Will you stop them or will you get lost in the dark?"))
	.loadLocstringsFromFile("creeperwoods", [] {LOCTABLE_FROMFILE_GAME("creeperwoods", "creeperwoodsLabels", "Decor/Text/creeperwoodsLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::Overworld()))
	;

const MissionDef& CreepyCryptDaily = createTrial(ELevelNames::creepycryptdaily, ELevelNames::creepycrypt, ELevelVariationType::daily);
const MissionDef& CreepyCryptWeekly = createTrial(ELevelNames::creepycryptweekly, ELevelNames::creepycrypt, ELevelVariationType::weekly);
const MissionDef& CreepyCryptSeasonal = createTrial(ELevelNames::creepycryptseasonal, ELevelNames::creepycrypt, ELevelVariationType::seasonal);

const MissionDef& WoodlandMansion = create(ELevelNames::woodlandmansion)
.name(LOCTEXT("woodlandmansion_name", "Woodland Mansion"))
.minimalThreatLevel(EThreatLevel::Threat_2)
.storyTitle(LOCTEXT("woodlandmansion_title", "Another part of the Woods"))
.storyContents(LOCTEXT("woodlandmansion_contents", "Deep in the woods the Redstone Speakers reside. Undisturbed, they strive to split the block in an effort to create ever more advanced redstone contraptions to serve the Arch-Illager's army."))
.loadLocstringsFromFile("creeperwoods", [] {LOCTABLE_FROMFILE_GAME("creeperwoods", "creeperwoodsLabels", "Decor/Text/creeperwoodsLabels.csv"); })
.neverOfInterest()
;

const MissionDef& SpiderCave = create(ELevelNames::spidercave)
.name(LOCTEXT("spidercave_name", "Spider Cave"))
.minimalThreatLevel(EThreatLevel::Threat_2)
.storyTitle(LOCTEXT("spidercave_title", "Another part of the Woods"))
.storyContents(LOCTEXT("spidercave_contents", "Deep in the woods the Redstone Speakers reside. Undisturbed, they strive to split the block in an effort to create ever more advanced redstone contraptions to serve the Arch-Illager's army."))
.loadLocstringsFromFile("creeperwoods", [] {LOCTABLE_FROMFILE_GAME("creeperwoods", "creeperwoodsLabels", "Decor/Text/creeperwoodsLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors())) 	
.neverOfInterest()
;

const MissionDef& CactiCanyon = create(ELevelNames::cacticanyon)
	.name(LOCTEXT("cacticanyon_name", "Cacti Canyon"))
	.storyTitle(LOCTEXT("cacticanyon_title", "Lost in a Sea of Sand"))
	.addRequiredMission(ELevelNames::creeperwoods)
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.storyContents(LOCTEXT("cacticanyon_contents", "Those who wish to reach the desert have to traverse the winding paths and trails of Cacti Canyon. A complex network of tunnels, chasms, and caves have led even the most level-headed adventurers astray. Those who wish to survive this sandy labyrinth must rely on their wits."))
	.loadLocstringsFromFile("cacticanyon", [] {LOCTABLE_FROMFILE_GAME("cacticanyon", "cacticanyonLabels", "Decor/Text/cacticanyonLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 98, 98, 128, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::SkeletonHorseman(), em::Enderman().d2()}))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Cacti_Canyon2.Loading_Screen_Cacti_Canyon2"))	
	.selectOrder(600)
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	;

const MissionDef& CactiCanyonDaily = createTrial(ELevelNames::cacticanyondaily, ELevelNames::cacticanyon, ELevelVariationType::daily);
const MissionDef& CactiCanyonWeekly = createTrial(ELevelNames::cacticanyonweekly, ELevelNames::cacticanyon, ELevelVariationType::weekly);
const MissionDef& CactiCanyonSeasonal = createTrial(ELevelNames::cacticanyonseasonal, ELevelNames::cacticanyon, ELevelVariationType::seasonal);

const MissionDef& DesertTemple = create(ELevelNames::deserttemple)
	.name(LOCTEXT("deserttemple_name", "Desert Temple"))
	.addRequiredMission(ELevelNames::cacticanyon)	
	.minimalThreatLevel(EThreatLevel::Threat_3)
	.storyTitle(LOCTEXT("deserttemple_title", "The Nameless Kingdom"))
	.storyContents(LOCTEXT("deserttemple_contents", "The wind-blown sands of the Desert Temple have wiped away the traces of a vast and ancient kingdom. Yet secretive power still lingers here. The enchanters of this ancient, nameless kingdom once practiced necromancy, magic that summons the dead.\n\nNow, generations later, the Illagers have arrived to try and pry power from these dusty tombs. Who knows what power they may find, but something else also protects these dusty crypts..."))
	.loadLocstringsFromFile("deserttemple", [] {LOCTABLE_FROMFILE_GAME("deserttemple", "deserttempleLabels", "Decor/Text/deserttempleLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 224, 165, 108, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::Enderman()}))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Desert_Temple.Loading_Screen_Desert_Temple"))
	.selectOrder(700)
	.boss()
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	;

const MissionDef& DesertTempleDaily = createTrial(ELevelNames::deserttempledaily, ELevelNames::deserttemple, ELevelVariationType::daily);
const MissionDef& DesertTempleWeekly = createTrial(ELevelNames::deserttempleweekly, ELevelNames::deserttemple, ELevelVariationType::weekly);
const MissionDef& DesertTempleSeasonal = createTrial(ELevelNames::deserttempleseasonal, ELevelNames::deserttemple, ELevelVariationType::seasonal);

const MissionDef& LowerTemple = createSecret(ELevelNames::lowertemple, ELevelNames::deserttemple)
	.name(LOCTEXT("deserttemplebonus_name", "Lower Temple"))
	.storyTitle(LOCTEXT("deserttemplebonus_title", "Tombs Unknown"))
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.storyContents(LOCTEXT("deserttemplebonus_contents", "Nobody knows how much further down this temple goes. You have to venture deeper into the slumbering catacombs and make sure that there are no lingering Illagers."))
	.loadLocstringsFromFile("lowertemple", [] {LOCTABLE_FROMFILE_GAME("lowertemple", "lowertempleLabels", "Decor/Text/lowertempleLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2
	.eventMobs(em::AddRemainingApocalypseMobs({ em::Enderman().d2() }))
	;

const MissionDef& LowerTempleDaily = createTrial(ELevelNames::lowertempledaily, ELevelNames::lowertemple, ELevelVariationType::daily);
const MissionDef& LowerTempleWeekly = createTrial(ELevelNames::lowertempleweekly, ELevelNames::lowertemple, ELevelVariationType::weekly);
const MissionDef& LowerTempleSeasonal = createTrial(ELevelNames::lowertempleseasonal, ELevelNames::lowertemple, ELevelVariationType::seasonal);

const MissionDef& FieryForge = create(ELevelNames::fieryforge)
	.name(LOCTEXT("fieryforge_name", "Fiery Forge"))
	.addRequiredMission(ELevelNames::mooncorecaverns)
	.minimalThreatLevel(EThreatLevel::Threat_3)
	.storyTitle(LOCTEXT("fieryforge_title", "Monstrosities of War"))
	.storyContents(LOCTEXT("fieryforge_contents", "The Fiery Forge, the beating heart of the Illager war-machine, is the source of the Arch-Illager army's power. Vindicator axes, redstone golems, and enchanter spellbooks all originate from the fires of this vile place.\n\nThis fortress has long been impossible to breach, and none would even dare try. But even a place like this must have a secret entrance..."))
	.loadLocstringsFromFile("fieryforge", [] {LOCTABLE_FROMFILE_GAME("fieryforge", "fieryforgeLabels", "Decor/Text/fieryforgeLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::NewCave())) //D11.PS RETLAMBDA2
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 49, 72, 88, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::RedstoneGolem()}))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Fiery_Forge.Loading_Screen_Fiery_Forge"))
	.selectOrder(500)
	.boss()
	;

const MissionDef& FieryForgeDaily = createTrial(ELevelNames::fieryforgedaily, ELevelNames::fieryforge, ELevelVariationType::daily);
const MissionDef& FieryForgeWeekly = createTrial(ELevelNames::fieryforgeweekly, ELevelNames::fieryforge, ELevelVariationType::weekly);
const MissionDef& FieryForgeSeasonal = createTrial(ELevelNames::fieryforgeseasonal, ELevelNames::fieryforge, ELevelVariationType::seasonal);

const MissionDef& HighblockHalls = create(ELevelNames::highblockhalls)
	.name(LOCTEXT("highblockhalls_name", "Highblock Halls"))
	.addRequiredMission(ELevelNames::fieryforge)
	.addRequiredMission(ELevelNames::deserttemple)
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.storyTitle(LOCTEXT("highblockhalls_title", "Crashing the Party!"))
	.storyContents(LOCTEXT("highblockhalls_contents", "The Arch-Illager and his minions are gathering in the grand halls of Highblock Keep to celebrate their recent victories. If we strike while the Illagers are all in one place, we could turn the tide of this conflict."))
	.loadLocstringsFromFile("highblockhalls", [] {LOCTABLE_FROMFILE_GAME("highblockhalls", "highblockhallsLabels", "Decor/Text/highblockhallsLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2	
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 66, 75, 60, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::Evoker(), em::RedstoneGolem().d2()}))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Highblock_Halls.Loading_Screen_Highblock_Halls"))
	.selectOrder(800)
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	;

const MissionDef& HighblockHallsDaily = createTrial(ELevelNames::highblockhallsdaily, ELevelNames::highblockhalls, ELevelVariationType::daily);
const MissionDef& HighblockHallsWeekly = createTrial(ELevelNames::highblockhallsweekly, ELevelNames::highblockhalls, ELevelVariationType::weekly);
const MissionDef& HighblockHallsSeasonal = createTrial(ELevelNames::highblockhallsseasonal, ELevelNames::highblockhalls, ELevelVariationType::seasonal);

const MissionDef& Underhalls = createSecret(ELevelNames::underhalls, ELevelNames::highblockhalls)
	.name(LOCTEXT("highblockhallsbonus_name", "Underhalls"))
	.storyTitle(LOCTEXT("highblockhallsbonus_title", "Under Lock and Key"))
	.minimalThreatLevel(EThreatLevel::Threat_5)
	.storyContents(LOCTEXT("highblockhallsbonus_contents", "No keep is complete without a dungeon and Highblock Halls is no exception. Who knows what the Arch-Illager has hidden away down there? There's only one way to find out..."))
	.loadLocstringsFromFile("PrisonDungeon", [] {LOCTABLE_FROMFILE_GAME("PrisonDungeon", "UnderHallsLabels", "Decor/Text/UnderHallsLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2	
	;

const MissionDef& UnderhallsDaily = createTrial(ELevelNames::underhallsdaily, ELevelNames::underhalls, ELevelVariationType::daily);
const MissionDef& UnderhallsWeekly = createTrial(ELevelNames::underhallsweekly, ELevelNames::underhalls, ELevelVariationType::weekly);
const MissionDef& UnderhallsSeasonal = createTrial(ELevelNames::underhallsseasonal, ELevelNames::underhalls, ELevelVariationType::seasonal);

const MissionDef& MooncoreCaverns = create(ELevelNames::mooncorecaverns)
	.name(LOCTEXT("mooncorecaverns_name", "Redstone Mines"))
	.addRequiredMission(ELevelNames::creeperwoods)
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.storyTitle(LOCTEXT("mooncorecaverns_title", "The End of the Line"))
	.storyContents(LOCTEXT("mooncorecaverns_contents", "The Illagers have finally accomplished their goal: to tear the redstone from the very heart of the mountains. With that kind of power, no one in the Overworld will be able to stand in their way. Time is running out to stop the Arch-Illager before it is too late."))
	.loadLocstringsFromFile("mooncorecaverns", [] {LOCTABLE_FROMFILE_GAME("mooncorecaverns", "mooncorecavernsLabels", "Decor/Text/mooncorecavernsLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::NewCave())) //D11.PS RETLAMBDA2
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 49, 72, 88, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::RedstoneGolem().d2(), em::Enderman().d2(), em::Evoker() }))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Redstone_Mines.Loading_Screen_Redstone_Mines"))
	.selectOrder(400)
	.boss()
	;

const MissionDef& MooncoreCavernsDaily = createTrial(ELevelNames::mooncorecavernsdaily, ELevelNames::mooncorecaverns, ELevelVariationType::daily);
const MissionDef& MooncoreCavernsWeekly = createTrial(ELevelNames::mooncorecavernsweekly, ELevelNames::mooncorecaverns, ELevelVariationType::weekly);
const MissionDef& MooncoreCavernsSeasonal = createTrial(ELevelNames::mooncorecavernsseasonal, ELevelNames::mooncorecaverns, ELevelVariationType::seasonal);

const MissionDef& ObsidianPinnacle = create(ELevelNames::obsidianpinnacle)
	.name(LOCTEXT("obsidianpinnacle_name", "Obsidian Pinnacle"))
	.addRequiredMission(ELevelNames::highblockhalls)
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.storyTitle(LOCTEXT("obsidianpinnacle_title", "The Eye of the Storm"))
	.storyContents(LOCTEXT("obsidianpinnacle_contents", "It is time to end the Arch-Illager's reign of terror. We've trapped the little tyrant in the highest tower of the castle, and there is nowhere left for him to run."))
	.loadLocstringsFromFile("obsidianpinnacle", [] {LOCTABLE_FROMFILE_GAME("obsidianpinnacle", "obsidianpinnacleLabels", "Decor/Text/obsidianpinnacleLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors())) //D11.PS RETLAMBDA2	
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 171, 82, 41, 255 }))
	.eventMobs({em::RedstoneGolem(), em::Evoker(), em::Enderman().d3() }) // We don't add Horsemen here
	.endVideo(CreateEndVideoFunctor<UObsidianPinnacleOutroVideo>())
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Obsidian_Pinnacle.Loading_Screen_Obsidian_Pinnacle"))
	.selectOrder(900)
	.boss()
	;

const MissionDef& ObsidianPinnacleDaily = createTrial(ELevelNames::obsidianpinnacledaily, ELevelNames::obsidianpinnacle, ELevelVariationType::daily);
const MissionDef& ObsidianPinnacleWeekly = createTrial(ELevelNames::obsidianpinnacleweekly, ELevelNames::obsidianpinnacle, ELevelVariationType::weekly);
const MissionDef& ObsidianPinnacleSeasonal = createTrial(ELevelNames::obsidianpinnacleseasonal, ELevelNames::obsidianpinnacle, ELevelVariationType::seasonal);

const MissionDef& PumpkinPastures = create(ELevelNames::pumpkinpastures)
	.name(LOCTEXT("pumpkinpastures_name", "Pumpkin Pastures"))
	.addRequiredMission(ELevelNames::creeperwoods)
	.storyTitle(LOCTEXT("pumpkinpastures_title", "The Last Homely Hearth"))
	.storyContents(LOCTEXT("pumpkinpastures_contents", "The Illager raids have reached the once-quiet fields of Pumpkin Pastures. As the Villagers were dragged off in chains and everything burned, it seemed all was lost. But one village still stands in the chaos. That last homely hearth remains a beacon of hope, but they need our help!"))
	.loadLocstringsFromFile("pumpkinpastures", [] {LOCTABLE_FROMFILE_GAME("pumpkinpastures", "pumpkinpasturesLabels", "Decor/Text/pumpkinpasturesLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::Overworld()))
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 203, 105, 47, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::Evoker(), em::Enderman().d2(), em::SkeletonHorseman().d2()}))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Pumpkin_Pastures.Loading_Screen_Pumpkin_Pastures"))
	.selectOrder(200)
	.boss()
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	;

const MissionDef& PumpkinPasturesDaily = createTrial(ELevelNames::pumpkinpasturesdaily, ELevelNames::pumpkinpastures, ELevelVariationType::daily);
const MissionDef& PumpkinPasturesWeekly = createTrial(ELevelNames::pumpkinpasturesweekly, ELevelNames::pumpkinpastures, ELevelVariationType::weekly);
const MissionDef& PumpkinPasturesSeasonal = createTrial(ELevelNames::pumpkinpasturesseasonal, ELevelNames::pumpkinpastures, ELevelVariationType::seasonal);

const MissionDef& ArchHaven = createSecret(ELevelNames::archhaven, ELevelNames::pumpkinpastures)
	.name(LOCTEXT("pumpkinpasturesbonus_name", "Arch Haven"))
	.storyTitle(LOCTEXT("pumpkinpasturesbonus_title", "A Hidden Stash"))
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.storyContents(LOCTEXT("pumpkinpasturesbonus_contents", "What's this? You've stumbled upon an outpost used by the Arch-Illager and his wretched band of ill willing bootlickers! Surely stealing his secret plans and grabbing his ill-gotten loot will set him back considerably!"))
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors()))
	.loadLocstringsFromFile("archhaven", [] {LOCTABLE_FROMFILE_GAME("archhaven", "archhavenLabels", "Decor/Text/archhavenLabels.csv"); })
	.eventMobs(em::AddRemainingApocalypseMobs({ em::Evoker()/*, em::Enderman()*/, em::SkeletonHorseman().d2() }))
	;

const MissionDef& ArchHavenDaily = createTrial(ELevelNames::archhavendaily, ELevelNames::archhaven, ELevelVariationType::daily);
const MissionDef& ArchHavenWeekly = createTrial(ELevelNames::archhavenweekly, ELevelNames::archhaven, ELevelVariationType::weekly);
const MissionDef& ArchHavenSeasonal = createTrial(ELevelNames::archhavenseasonal, ELevelNames::archhaven, ELevelVariationType::seasonal);

const MissionDef& SlimySewers = create(ELevelNames::slimysewers)
	.permanentlyDisabled()
	.name(LOCTEXT("slimysewers_name", "Slimy Sewers"))
	.storyTitle(LOCTEXT("slimysewers_title", "A Smelly Situation"))
	.storyContents(LOCTEXT("slimysewers_contents", "The only way to enter Highblock Keep undetected is to crawl through the slimy sewers. Disgusting, dangerous, and a bit smelly, it is sadly the safest way into the heavily guarded Illager keep. "))
	.loadLocstringsFromFile("slimysewers", [] {LOCTABLE_FROMFILE_GAME("slimysewers", "slimysewersLabels", "Decor/Text/slimysewersLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::Overworld()))
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 76, 94, 85, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::RedstoneGolem().d2()}))
	;

const MissionDef& SlimySewersDaily = createTrial(ELevelNames::slimysewersdaily, ELevelNames::slimysewers, ELevelVariationType::daily).disabled();
const MissionDef& SlimySewersWeekly = createTrial(ELevelNames::slimysewersweekly, ELevelNames::slimysewers, ELevelVariationType::weekly).disabled();
const MissionDef& SlimySewersSeasonal = createTrial(ELevelNames::slimysewersseasonal, ELevelNames::slimysewers, ELevelVariationType::seasonal).disabled();

const MissionDef& SoggySwamp = create(ELevelNames::soggyswamp)
	.name(LOCTEXT("soggyswamp_name", "Soggy Swamp"))
	.addRequiredMission(ELevelNames::creeperwoods)
	.storyTitle(LOCTEXT("soggyswamp_title", "A Perilous Potion"))
	.storyContents(LOCTEXT("soggyswamp_contents", "The witches of Soggy Swamp have polluted the land with their wicked brews. We can't let their potions fall into the hands of the Arch-Illager, who would surely use that power for no good."))
	.loadLocstringsFromFile("soggyswamp", [] {LOCTABLE_FROMFILE_GAME("soggyswamp", "soggyswampLabels", "Decor/Text/soggyswampLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::Overworld()))
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 183, 165, 141, 255 }))
	.eventMobs(em::AddRemainingApocalypseMobs({em::Enderman(), em::SkeletonHorseman().d2()}))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Soggy_Swamp.Loading_Screen_Soggy_Swamp"))
	.selectOrder(300)
	.waterMaterialOverride("SwampWater")
	.boss()
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	;

const MissionDef& SoggySwampDaily = createTrial(ELevelNames::soggyswampdaily, ELevelNames::soggyswamp, ELevelVariationType::daily);
const MissionDef& SoggySwampWeekly = createTrial(ELevelNames::soggyswampweekly, ELevelNames::soggyswamp, ELevelVariationType::weekly);
const MissionDef& SoggySwampSeasonal = createTrial(ELevelNames::soggyswampseasonal, ELevelNames::soggyswamp, ELevelVariationType::seasonal);

const MissionDef& SoggyCave = createSecret(ELevelNames::soggycave, ELevelNames::soggyswamp)
	.name(LOCTEXT("soggyswampbonus_name", "Soggy Cave"))
	.storyTitle(LOCTEXT("soggyswampbonus_title", "The Lost Ruins"))
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.storyContents(LOCTEXT("soggyswampbonus_contents", "There are plenty of treasures hidden in the swamp's caves. Claim them before the Arch-Illager can!"))
	.loadLocstringsFromFile("soggyswamp", [] {LOCTABLE_FROMFILE_GAME("soggyswamp", "soggyswampLabels", "Decor/Text/soggyswampLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::Overworld()))
	;

const MissionDef& SoggyCaveDaily = createTrial(ELevelNames::soggycavedaily, ELevelNames::soggycave, ELevelVariationType::daily);
const MissionDef& SoggyCaveWeekly = createTrial(ELevelNames::soggycaveweekly, ELevelNames::soggycave, ELevelVariationType::weekly);
const MissionDef& SoggyCaveSeasonal = createTrial(ELevelNames::soggycaveseasonal, ELevelNames::soggycave, ELevelVariationType::seasonal);

const MissionDef& SquidCoast = create(ELevelNames::squidcoast)
	.name(LOCTEXT("squidcoast_name", "Squid Coast"))
	.storyTitle(LOCTEXT("squidcoast_title", "A Village Under Attack"))
	.storyContents(LOCTEXT("squidcoast_contents", "The village is under attack by the Arch-Illager's evil minions. Someone needs to save the Villagers before it is too late!"))	
	.loadLocstringsFromFile("squidcoast", [] {LOCTABLE_FROMFILE_GAME("squidcoast", "squidcoastLabels", "Decor/Text/squidcoastLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors())) //D11.PS RETLAMBDA2	
	.unlockedThreatLevelByDefault()
	.dropsDisabled()
	.salvageDisabled()
	.tutorial()
	.infiniteLives()
	.fixedThreatLevel(EThreatLevel::Threat_1)
	.maximalDifficulty(EGameDifficulty::Difficulty_1)
	.hiddenThreatLevelSelector()
	//Inline lambda to get around static init order.
	.rewards(game::item::generator::Config([](const auto& state) { return state.item.getId() == game::item::type::FishingRod.getId(); }))
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 105, 147, 66, 255 }))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Screen_Squid_Coast.Loading_Screen_Squid_Coast"))
	.selectOrder(0)
	;

const MissionDef& MooshroomIsland = create(ELevelNames::mooshroomisland)
	.setNeedsSpecificUnlock(true)
	.setTheme(EMissionTheme::Secret)
	.name(LOCTEXT("mooshroomisland_name", "???"))
	.storyTitle(LOCTEXT("mooshroomisland_title", "Moo"))
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.storyContents(LOCTEXT("mooshroomisland_contents", "Moo? Moo!"))
	.loadLocstringsFromFile("mooshroomisland", [] {LOCTABLE_FROMFILE_GAME("mooshroomisland", "mooshroomislandLabels", "Decor/Text/mooshroomislandLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::Overworld()))
	.selectOrder(std::numeric_limits<int>::max()-10)
	.rewards(game::item::generator::Config([](const auto& state) { return state.item.getId() == game::item::type::RainbowGrass.getId(); }))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingscreen_subdungeon.loadingscreen_subdungeon"))
	.boss()
	;


const MissionDef& MooshroomIslandDaily = createTrial(ELevelNames::mooshroomislanddaily, ELevelNames::mooshroomisland, ELevelVariationType::daily);
const MissionDef& MooshroomIslandWeekly = createTrial(ELevelNames::mooshroomislandweekly, ELevelNames::mooshroomisland, ELevelVariationType::weekly);
const MissionDef& MooshroomIslandSeasonal = createTrial(ELevelNames::mooshroomislandseasonal, ELevelNames::mooshroomisland, ELevelVariationType::seasonal);

// DLC - Mojang

const MissionDef& CrimsonForest = createSecret(ELevelNames::crimsonforest, ELevelNames::warpedforest)
.name(LOCTEXT("crimsonforest_name", "Crimson Forest"))
.storyTitle(LOCTEXT("crimsonforest_title", "Crimson Shadows"))
.storyContents(LOCTEXT("crimsonforest_contents", "Twisting paths through the vermillion foliage will bring you to the brink of vast chasms and lead you through ruins of old. There is a strange beauty here, but never forget that even one wrong step could lead to your doom."))
.loadLocstringsFromFile("crimsonforest", [] {LOCTABLE_FROMFILE_GAME("crimsonforest", "crimsonforestLabels", "Decor/Text/crimsonforestLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors()))
.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
.nightName(LOCTEXT("NightName_Nether", "The Embers Fade"))
.eventMobs({ em::Ghast() })
.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Crimson_Forest.Loading_Crimson_Forest"))
.selectOrder(5000)
.requiredDLC(EDLCName::Nether)
;

const MissionDef& CrimsonForestDaily = createTrial(ELevelNames::crimsonforestdaily, ELevelNames::crimsonforest, ELevelVariationType::daily);
const MissionDef& CrimsonForestWeekly = createTrial(ELevelNames::crimsonforestweekly, ELevelNames::crimsonforest, ELevelVariationType::weekly);
const MissionDef& CrimsonForestSeasonal = createTrial(ELevelNames::crimsonforestseasonal, ELevelNames::crimsonforest, ELevelVariationType::seasonal);

const MissionDef& BasaltDeltas = create(ELevelNames::basaltdeltas)
.name(LOCTEXT("basaltdeltas_name", "Basalt Deltas"))
.storyTitle(LOCTEXT("basaltdeltas_title", "Volcanic Pathways"))
.storyContents(LOCTEXT("basaltdeltas_contents", "Deep within the Nether, there are the Basalt Deltas, a violent and volcanic region where lava flows in abundance. Your safe passage relies on moving platforms from an unknowable time, trusting that their secretive sources of power will carry you safely to your destination."))
.loadLocstringsFromFile("basaltdeltas", [] {LOCTABLE_FROMFILE_GAME("basaltdeltas", "basaltdeltasLabels", "Decor/Text/basaltdeltasLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors()))
.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
.addRequiredMission(ELevelNames::netherwastes)
.nightName(LOCTEXT("NightName_Nether", "The Embers Fade"))
.eventMobs({ em::Ghast() })
.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Basalt_Deltas.Loading_Basalt_Deltas"))
.selectOrder(5100)
.requiredDLC(EDLCName::Nether)
;

const MissionDef& BasaltDeltasDaily = createTrial(ELevelNames::basaltdeltasdaily, ELevelNames::basaltdeltas, ELevelVariationType::daily);
const MissionDef& BasaltDeltasWeekly = createTrial(ELevelNames::basaltdeltasweekly, ELevelNames::basaltdeltas, ELevelVariationType::weekly);
const MissionDef& BasaltDeltasSeasonal = createTrial(ELevelNames::basaltdeltasseasonal, ELevelNames::basaltdeltas, ELevelVariationType::seasonal);

const MissionDef& SoulsandValley = createSecret(ELevelNames::soulsandvalley, ELevelNames::crimsonforest)
.name(LOCTEXT("soulsandvalley_name", "Soul Sand Valley"))
.storyTitle(LOCTEXT("soulsandvalley_title", "Dark Dunes"))
.storyContents(LOCTEXT("soulsandvalley_contents", "The Soul Sand Valley is a bone-chilling part of the Nether that even the boldest of warriors dread to tread. Gather your wits and hold your weapon close as you cross the shifting sands."))
.loadLocstringsFromFile("SoulsandValley", [] {LOCTABLE_FROMFILE_GAME("SoulsandValley", "SoulsandValleyLabels", "Decor/Text/SoulsandValleyLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 0, 0, 0, 4, 3, 3, 3, 2})))
.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
.nightName(LOCTEXT("NightName_Nether", "The Embers Fade"))
.eventMobs({ em::Enderman(), em::Ghast() })
.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Soulsand_Valley.Loading_Soulsand_Valley"))
.selectOrder(5200)
.requiredDLC(EDLCName::Nether)
;

const MissionDef& SoulsandValleyDaily = createTrial(ELevelNames::soulsandvalleydaily, ELevelNames::soulsandvalley, ELevelVariationType::daily);
const MissionDef& SoulsandValleyWeekly = createTrial(ELevelNames::soulsandvalleyweekly, ELevelNames::soulsandvalley, ELevelVariationType::weekly);
const MissionDef& SoulsandValleySeasonal = createTrial(ELevelNames::soulsandvalleyseasonal, ELevelNames::soulsandvalley, ELevelVariationType::seasonal);

const MissionDef& WarpedForest = create(ELevelNames::warpedforest)
.name(LOCTEXT("warpedforest_name", "Warped Forest"))
.storyTitle(LOCTEXT("warpedforest_title", "The Warped Way"))
.storyContents(LOCTEXT("warpedforest_contents", "There is only one way in and out of the Nether: portals. With nothing but a broken portal at your back, you'll need to hope that the Warped Forest is hiding a new way to make it home. Watch your step, for these are dark woods and Endermen lurk within the shadows."))
.loadLocstringsFromFile("WarpedForest", [] {LOCTABLE_FROMFILE_GAME("WarpedForest", "WarpedForestLabels", "Decor/Text/WarpedForestLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors()))
.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
.addRequiredMission(ELevelNames::netherwastes)
.nightName(LOCTEXT("NightName_Nether", "The Embers Fade"))
.eventMobs({ em::Enderman(), em::Ghast().d2() })
.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Warped_Forest.Loading_Warped_Forest"))
.selectOrder(5300)
.requiredDLC(EDLCName::Nether)
;

const MissionDef& WarpedForestDaily = createTrial(ELevelNames::warpedforestdaily, ELevelNames::warpedforest, ELevelVariationType::daily);
const MissionDef& WarpedForestWeekly = createTrial(ELevelNames::warpedforestweekly, ELevelNames::warpedforest, ELevelVariationType::weekly);
const MissionDef& WarpedForestSeasonal = createTrial(ELevelNames::warpedforestseasonal, ELevelNames::warpedforest, ELevelVariationType::seasonal);

const MissionDef& NetherWastes = create(ELevelNames::netherwastes)
.name(LOCTEXT("netherwastes_name", "Nether Wastes"))
.storyTitle(LOCTEXT("netherwastes_title", "Infinite Wasteland"))
.storyContents(LOCTEXT("netherwastes_contents", "Any journey through the Nether begins in the Nether Wastes. The endless sprawl is scattered with strange relics that break up the bleak landscape, jutting forth from somewhere beneath the surface."))
.loadLocstringsFromFile("netherwastes", [] {LOCTABLE_FROMFILE_GAME("netherwastes", "netherwastesLabels", "Decor/Text/netherwastesLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors()))
.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
.nightName(LOCTEXT("NightName_Nether", "The Embers Fade"))
.eventMobs({ em::Enderman().d2(), em::Ghast() })
.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_NetherWastes.Loading_NetherWastes"))
.selectOrder(5100)
.requiredDLC(EDLCName::Nether)
;

const MissionDef& NetherWastesDaily = createTrial(ELevelNames::netherwastesdaily, ELevelNames::netherwastes, ELevelVariationType::daily);
const MissionDef& NetherWastesWeekly = createTrial(ELevelNames::netherwastesweekly, ELevelNames::netherwastes, ELevelVariationType::weekly);
const MissionDef& NetherWastesSeasonal = createTrial(ELevelNames::netherwastesseasonal, ELevelNames::netherwastes, ELevelVariationType::seasonal);

const MissionDef& NetherFortress = createSecret(ELevelNames::netherfortress, ELevelNames::basaltdeltas)
.name(LOCTEXT("netherfortress_name", "Nether Fortress"))
.storyTitle(LOCTEXT("netherfortress_title", "River of Fire"))
.storyContents(LOCTEXT("netherfortress_contents", "The Nether is a place of scarce resources, but one thing is abundant: lava. All those who want to survive must find a way around its scalding surface. Finding a safe path to reach your goal is not for the faint of heart or those afraid of a little hard work."))
.loadLocstringsFromFile("netherfortress", [] {LOCTABLE_FROMFILE_GAME("netherfortress", "netherfortressLabels", "Decor/Text/netherfortressLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2 }))) 
.mapColor(GenerateColorOffsetsFromBase(FColor{ 171, 82, 41, 255 }))
.nightName(LOCTEXT("NightName_Nether", "The Embers Fade"))
.eventMobs({ em::Enderman().d3(), em::Ghast().d2() })
.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Nether_Fortress.Loading_Nether_Fortress"))
.selectOrder(5100)
.requiredDLC(EDLCName::Nether)
;

const MissionDef& NetherFortressDaily = createTrial(ELevelNames::netherfortressdaily, ELevelNames::netherfortress, ELevelVariationType::daily);
const MissionDef& NetherFortressWeekly = createTrial(ELevelNames::netherfortressweekly, ELevelNames::netherfortress, ELevelVariationType::weekly);
const MissionDef& NetherFortressSeasonal = createTrial(ELevelNames::netherfortressseasonal, ELevelNames::netherfortress, ELevelVariationType::seasonal);

const MissionDef& NetherHyperMission = create(ELevelNames::netherhypermission)
	.hyperMission()
	.setTheme(EMissionTheme::Hypermission)
	.name(LOCTEXT("netherhypermission_name", "Ancient Hunt"))
	.confirmStartTitle(LOCTEXT("netherhypermission_confirm_title", "The Ancient Hunt is Ready"))
	.confirmStartWarning(LOCTEXT("netherhypermission_confirm_warning", "Starting the mission will destroy the offered items and Enchantment Points."))
	.storyTitle(LOCTEXT("netherhypermission_title", "The Ancient Hunt begins"))
	.storyContents(LOCTEXT("netherhypermission_contents", "Will you claim treasures beyond compare or be defeated by the Ancient mobs who are drawn to your power?"))
	.loadLocstringsFromFile("hypermission", [] {LOCTABLE_FROMFILE_GAME("hypermission", "hypermissionLabels", "Decor/Text/hypermissionLabels.csv"); })
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 })))
	.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
	.allowContinue()
	.neverOfInterest()
	.requiredOfferedItemCount(1)
	.requiredOfferedDisplayItemPower(100)
	.requiredNumAncientMobTypes(1)
	.metaScorer(JustMetaCodePath())
	.rewards(itemgen::Config([](const auto& state) { return state.item.getId() == game::item::type::Gold.getId(); }))
	.overrideRewardCount(10)
	.requiredEndGameContent(EEndGameContentType::HyperMission)
	.grantsUnlockKey([]() { return UPiglinMerchantDef::StaticClass()->GetDefaultObject<UPiglinMerchantDef>()->GetUnlockProgressKey(); })
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loading_Ancient_Hunt.Loading_Ancient_Hunt"))
	.selectOrder(5100)
	;

#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
// Test - Special Tiles
const MissionDef& SpecialTilesHub = create(ELevelNames::specialtileshub)
.name(LOCTEXT("specialtileshub_name", "Special Tiles Hub"))
.storyTitle(LOCTEXT("specialtileshub_title", "Welcome to the Special Tiles"))
.storyContents(LOCTEXT("specialtileshub_contents", "Hello! Thank you for testing the Special Tiles!"))
.loadLocstringsFromFile("specialtileshub", [] {LOCTABLE_FROMFILE_GAME("specialtileshub", "specialtileshubLabels", "Decor/Text/netherfortressLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 })))
.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingCreeperWoods.loadingCreeperWoods"))
.selectOrder(5100)
.requiredDLC(EDLCName::Nether)
;

// Test - HM Hub
const MissionDef& hm_hub = create(ELevelNames::hm_hub)
.name(LOCTEXT("hm_hub_name", "HM Tiles Hub"))
.storyTitle(LOCTEXT("hm_hub_title", "Welcome to the Special Tiles"))
.storyContents(LOCTEXT("hm_hub_contents", "Hello! Thank you for testing the Special Tiles!"))
.loadLocstringsFromFile("hm_hub", [] {LOCTABLE_FROMFILE_GAME("hm_hub", "hm_hubLabels", "Decor/Text/netherfortressLabels.csv"); })
.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 })))
.mapColor(GenerateColorOffsetsFromBase(FColor{ 63, 90, 128, 255 }))
.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingCreeperWoods.loadingCreeperWoods"))
.selectOrder(5100)
.requiredDLC(EDLCName::Nether)
;
#endif

// D11.MD	=== D11 Mission Definitions ===


// D11.MD	=== Dingy Jungle ===
const MissionDef& DingyJungle = create(ELevelNames::dingyjungle)	
	.name(LOCTEXT("dingyjungle_name", "Dingy Jungle"))
	.storyTitle(LOCTEXT("dingyjungle_title", "Entangled in Vines"))
	.storyContents(LOCTEXT("dingyjungle_contents", "We've tracked the Orb's shard to the edge of a dense jungle where dangerous creatures creep out from under the thick canopy. What brave hero would dare cut through the twisting vines to reach the dark power that hides within?"))
	.loadLocstringsFromFile("dingyjungleLoctable", [] {LOCTABLE_FROMFILE_GAME("dingyjungleLoctable", "dingyjungleLabels", "Decor/Text/dingyjungleLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2	
	.eventMobs({  em::Enderman(), em::SkeletonHorseman().d2() })
	.metaScorer({
		Required(Equals(SubDungeons(), 2)),
		pick::First()
	})
	.selectOrder(2000)
	.requiredDLC(EDLCName::TheJungleAwakens)
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingscreen_DLC_DingyJungle.loadingscreen_DLC_DingyJungle"))
	;

const MissionDef& DingyJungleDaily = createTrial(ELevelNames::dingyjungledaily, ELevelNames::dingyjungle, ELevelVariationType::daily);
const MissionDef& DingyJungleWeekly = createTrial(ELevelNames::dingyjungleweekly, ELevelNames::dingyjungle, ELevelVariationType::weekly);
const MissionDef& DingyJungleSeasonal = createTrial(ELevelNames::dingyjungleseasonal, ELevelNames::dingyjungle, ELevelVariationType::seasonal);

// D11.MD	=== Overgrown Temple ===
const MissionDef& OvergrownTemple = create(ELevelNames::overgrowntemple)	
	.name(LOCTEXT("overgrowntemple_name", "Overgrown Temple"))
	.storyTitle(LOCTEXT("overgrowntemple_title", "Tangle in the Temple"))
	.storyContents(LOCTEXT("overgrowntemple_contents", "Deep within the jungle lies an overgrown temple, a place where nature has reclaimed its rightful place as ruler of the land. Could the shard have landed here?"))
	.loadLocstringsFromFile("overgrowntempleLoctable", [] {LOCTABLE_FROMFILE_GAME("overgrowntempleLoctable", "overgrowntempleLabels", "Decor/Text/overgrowntempleLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2	
	.eventMobs({ em::Enderman(), em::SkeletonHorseman().d2() })
	.selectOrder(2100)
	.metaScorer({
		Required(Equals(SubDungeons(), 2)),
		pick::First()
		})
	.addRequiredMission(ELevelNames::dingyjungle)
	.requiredDLC(EDLCName::TheJungleAwakens)
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingscreen_DLC_overgrownTemple.loadingscreen_DLC_overgrownTemple"))
	.boss()
	;

const MissionDef& OvergrownTempleDaily = createTrial(ELevelNames::overgrowntempledaily, ELevelNames::overgrowntemple, ELevelVariationType::daily);
const MissionDef& OvergrownTempleWeekly = createTrial(ELevelNames::overgrowntempleweekly, ELevelNames::overgrowntemple, ELevelVariationType::weekly);
const MissionDef& OvergrownTempleSeasonal = createTrial(ELevelNames::overgrowntempleseasonal, ELevelNames::overgrowntemple, ELevelVariationType::seasonal);

// D11.MD	=== Bamboo Bluff ===
const MissionDef& BambooBluff = createSecret(ELevelNames::bamboobluff, ELevelNames::dingyjungle)
	.name(LOCTEXT("bamboobluff_name", "Panda Plateau"))
	.storyTitle(LOCTEXT("bamboobluff_title", "Protect the Pandas"))
	.storyContents(LOCTEXT("bamboobluff_contents", "Within a secluded bamboo forest, perched on the face of a sprawling cliffside, a community of pandas have escaped the corrupting presence of the shard - until now. We need to fight back against the sprawling, strangling vines which have threatened to take over the homes of these lovable, defenseless pandas."))
	.loadLocstringsFromFile("bamboobluffLoctable", [] {LOCTABLE_FROMFILE_GAME("bamboobluffLoctable", "bamboobluffLabels", "Decor/Text/bamboobluffLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_5)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2	
	.eventMobs({ em::Enderman(), em::SkeletonHorseman().d2() })
	.selectOrder(2200)
	.metaScorer({
	Required(IsInRange(SubDungeons(), 2, 3)),
	pick::First()
	})
	.requiredDLC(EDLCName::TheJungleAwakens)
	;

const MissionDef& BambooBluffDaily = createTrial(ELevelNames::bamboobluffdaily, ELevelNames::bamboobluff, ELevelVariationType::daily);
const MissionDef& BambooBluffWeekly = createTrial(ELevelNames::bamboobluffweekly, ELevelNames::bamboobluff, ELevelVariationType::weekly);
const MissionDef& BambooBluffSeasonal = createTrial(ELevelNames::bamboobluffseasonal, ELevelNames::bamboobluff, ELevelVariationType::seasonal);

// D11.MD	=== Frozen Fjord ===
const MissionDef& FrozenFjord = create(ELevelNames::frozenfjord)	
	.name(LOCTEXT("frozenfjord_name", "Frosted Fjord"))
	.storyTitle(LOCTEXT("frozenfjord_title", "The Winding, Wintry Path"))
	.storyContents(LOCTEXT("frozenfjord_contents", "To reach the source of the creeping winter, we must journey into the deep reaches of the wintry wilderness, with only a frozen river as our guide. Icy paths and dangerous enemies attack through the wind and snow, but the distant sight of a lone fortress could serve as a sanctuary from the elements."))
	.loadLocstringsFromFile("frozenfjordLoctable", [] {LOCTABLE_FROMFILE_GAME("frozenfjordLoctable", "frozenfjordlabels", "Decor/Text/frozenfjordLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 15, 0, 0, 0, 0, 0, 0, 0, 0, 0 }))) //D11.PS RETLAMBDA2
	.eventMobs({ em::Illusioner()})
	.selectOrder(3000)
	.metaScorer({
		Required(IsInRange(SubDungeons(), 2, 4)),
		pick::First()
		})
	.requiredDLC(EDLCName::TheCreepingWinter)
	.icyBreath()
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingscreen_DLC_FrozenFjord.loadingscreen_DLC_FrozenFjord"))
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UFrozenFjordSlippyBlockTrigger::StaticClass() }; })
	.boss()
	;

const MissionDef& FrozenFjordDaily = createTrial(ELevelNames::frozenfjorddaily, ELevelNames::frozenfjord, ELevelVariationType::daily);
const MissionDef& FrozenFjordWeekly = createTrial(ELevelNames::frozenfjordweekly, ELevelNames::frozenfjord, ELevelVariationType::weekly);
const MissionDef& FrozenFjordSeasonal = createTrial(ELevelNames::frozenfjordseasonal, ELevelNames::frozenfjord, ELevelVariationType::seasonal);

// D11.MD	=== Lonely Fortress ===
const MissionDef& LonelyFortress = create(ELevelNames::lonelyfortress)	
	.name(LOCTEXT("lonelyfortress_name", "Lone Fortress"))
	.storyTitle(LOCTEXT("lonelyfortress_title", "Cold and Alone"))
	.storyContents(LOCTEXT("lonelyfortress_contents", "An isolated fortress seems to be the source of the never-ending winter. Worst of all, from somewhere deep within those lonely ramparts, something terrible screams out with rage and fury. We have to push forward through snow-covered hallways and collapsing towers to reach the heart of the storm."))
	.loadLocstringsFromFile("lonelyfortressLoctable", [] {LOCTABLE_FROMFILE_GAME("lonelyfortressLoctable", "lonelyfortressLabels", "Decor/Text/lonelyfortressLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 7, 2, 6, 8, 1, 15, 6, 2, 9, 1 }))) //D11.PS RETLAMBDA2
	.eventMobs({ em::Illusioner() })
	.selectOrder(3100)
	.metaScorer({
    Required({ IsInRange(BlocksArea(), 0, 300000), IsInRange(SubDungeons(), 0, 2)}),
		pick::First()
		})
	.addRequiredMission(ELevelNames::frozenfjord)
	.requiredDLC(EDLCName::TheCreepingWinter)
	.icyBreath()
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingscreen_DLC_LoneFortress.loadingscreen_DLC_LoneFortress"))
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { ULonelyFortressSlippyBlockTrigger::StaticClass() }; })
	.boss()
	;

const MissionDef& LonelyFortressDaily = createTrial(ELevelNames::lonelyfortressdaily, ELevelNames::lonelyfortress, ELevelVariationType::daily);
const MissionDef& LonelyFortressWeekly = createTrial(ELevelNames::lonelyfortressweekly, ELevelNames::lonelyfortress, ELevelVariationType::weekly);
const MissionDef& LonelyFortressSeasonal = createTrial(ELevelNames::lonelyfortressseasonal, ELevelNames::lonelyfortress, ELevelVariationType::seasonal);

// D11.MD	=== Lost Settlement ===
const MissionDef& LostSettlement = createSecret(ELevelNames::lostsettlement, ELevelNames::frozenfjord)	
	.name(LOCTEXT("lostsettlement_name", "Lost Settlement"))
	.storyTitle(LOCTEXT("lostsettlement_title", "Snowflake In A Blizzard"))
	.storyContents(LOCTEXT("lostsettlement_contents", "Long ago, a group of miners built a settlement on the shores of a great lake to harness the powerful energies hidden under the surface. But as time passed, the settlement was forgotten, and now the creeping winter has locked it away in ice. Who would be brave enough to face the elements and rediscover the magic of the this lost settlement?"))
	.loadLocstringsFromFile("lostsettlementLoctable", [] {LOCTABLE_FROMFILE_GAME("lostsettlementLoctable", "lostsettlementLabels", "Decor/Text/lostsettlementLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_5)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2
	.eventMobs({ em::Illusioner() })
	.selectOrder(3200)
	.metaScorer({
		Required(IsInRange(SubDungeons(), 1, 2)),
		pick::First()
		})	
	.requiredDLC(EDLCName::TheCreepingWinter)
	.icyBreath()
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { ULostSettlementSlippyBlockTrigger::StaticClass() }; })
	;

const MissionDef& LostSettlementDaily = createTrial(ELevelNames::lostsettlementdaily, ELevelNames::lostsettlement, ELevelVariationType::daily);
const MissionDef& LostSettlementWeekly = createTrial(ELevelNames::lostsettlementweekly, ELevelNames::lostsettlement, ELevelVariationType::weekly);
const MissionDef& LostSettlementSeasonal = createTrial(ELevelNames::lostsettlementseasonal, ELevelNames::lostsettlement, ELevelVariationType::seasonal);

const MissionDef& windsweptpeaks = create(ELevelNames::windsweptpeaks)
	.name(LOCTEXT("windsweptpeaks_name", "Windswept Peaks"))
	.storyTitle(LOCTEXT("windsweptpeaks_title", "Storm at the Summit")) 
	.storyContents(LOCTEXT("windsweptpeaks_contents", "A shard from the Orb of Dominance landed atop these mountain peaks, and now dark storms howl and snows swirl around the summit. The powerful winds carry the corrupting power of the shard, and if it is not stopped the world will surely be blown away by its power. Any hero who dares to free the mountaintop from the shard's grip will have to battle not just the elements but also the hordes of Illagers racing to the peak."))
	.loadLocstringsFromFile("windsweptpeaksLoctable", [] {LOCTABLE_FROMFILE_GAME("windsweptpeaksLoctable", "windsweptpeaksLabels", "Decor/Text/WindsweptPeaksLabels.csv"); })
	.requiredDLC(EDLCName::Mountains)
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.selectOrder(4000)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2
	.eventMobs({ em::Illusioner(), em::Evoker() })
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingscreen_DLC_WindsweptPeaks.loadingscreen_DLC_WindsweptPeaks"))
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UWindsweptPeaksSlippyBlockTrigger::StaticClass() }; })
	;

const MissionDef& windsweptpeaksDaily = createTrial(ELevelNames::windsweptpeaksdaily, ELevelNames::windsweptpeaks, ELevelVariationType::daily);
const MissionDef& windsweptpeaksWeekly = createTrial(ELevelNames::windsweptpeaksweekly, ELevelNames::windsweptpeaks, ELevelVariationType::weekly);
const MissionDef& windsweptpeaksSeasonal = createTrial(ELevelNames::windsweptpeaksseasonal, ELevelNames::windsweptpeaks, ELevelVariationType::seasonal);

// D11.MD	=== Gale Sanctum ===
const MissionDef& galesanctum = create(ELevelNames::galesanctum)
	.name(LOCTEXT("galesanctum_name", "Gale Sanctum "))
	.storyTitle(LOCTEXT("galesanctum_title", "A Temple Atop the Peaks"))
	.storyContents(LOCTEXT("galesanctum_contents", "A temple rises from the towering peaks where the winds are woven into the ancient sanctum's very structure. Harness the wind's power and claim undiscovered treasures, but mind the ancient protector within: the Tempest Golem."))
	.loadLocstringsFromFile("galesanctumLoctable", [] {LOCTABLE_FROMFILE_GAME("galesanctumLoctable", "galesanctumLabels", "Decor/Text/GaleSanctumLabels.csv"); })
	.requiredDLC(EDLCName::Mountains)
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.addRequiredMission(ELevelNames::windsweptpeaks)
	.selectOrder(4100)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2
	.eventMobs({ em::Illusioner(), em::Evoker() })
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/loadingscreen_DLC_GaleSanctum.loadingscreen_DLC_GaleSanctum"))
	.boss()
	;

const MissionDef& galesanctumDaily = createTrial(ELevelNames::galesanctumdaily, ELevelNames::galesanctum, ELevelVariationType::daily);
const MissionDef& galesanctumWeekly = createTrial(ELevelNames::galesanctumweekly, ELevelNames::galesanctum, ELevelVariationType::weekly);
const MissionDef& galesanctumSeasonal = createTrial(ELevelNames::galesanctumseasonal, ELevelNames::galesanctum, ELevelVariationType::seasonal);

// D11.MD	=== Endless Rampart ===
const MissionDef& endlessrampart = createSecret(ELevelNames::endlessrampart, ELevelNames::windsweptpeaks)
	.name(LOCTEXT("endlessrampart_name", "Colossal Rampart "))
	.storyTitle(LOCTEXT("endlessrampart_title", "Formidable Fortifications"))
	.storyContents(LOCTEXT("endlessrampart_contents", "A formidable wall twists along the base of these mighty peaks, stretching endlessly in every direction. These impressive defenses, abandoned long ago, have now been claimed by Illagers who seek to control the entire mountain range. Drive the Illagers from their newfound fortifications while you still can!"))
	.loadLocstringsFromFile("endlessrampartLoctable", [] {LOCTABLE_FROMFILE_GAME("endlessrampartLoctable", "endlessrampartLabels", "Decor/Text/EndlessRampartLabels.csv"); })
	.requiredDLC(EDLCName::Mountains)
	.minimalThreatLevel(EThreatLevel::Threat_5)
	.selectOrder(4200)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2
	.eventMobs({ em::Illusioner(), em::Evoker() })
	.addRequiredMission(ELevelNames::windsweptpeaks)
	;

const MissionDef& endlessrampartDaily = createTrial(ELevelNames::endlessrampartdaily, ELevelNames::endlessrampart, ELevelVariationType::daily);
const MissionDef& endlessrampartWeekly = createTrial(ELevelNames::endlessrampartweekly, ELevelNames::endlessrampart, ELevelVariationType::weekly);
const MissionDef& endlessrampartSeasonal = createTrial(ELevelNames::endlessrampartseasonal, ELevelNames::endlessrampart, ELevelVariationType::seasonal);

// D11.MD	=== Gauntlet of Gales ===
const MissionDef& gauntletgales = create(ELevelNames::gauntletgales)
	.name(LOCTEXT("gauntletgales_name", "Gauntlet of Gales"))
	.storyTitle(LOCTEXT("gauntletgales_title", "A Daunting Gauntlet"))
	.storyContents(LOCTEXT("gauntletgales_contents", "The puzzles and winding paths are aplenty in the Gauntlet of Gales. Enter the sanctum and complete the challenges to say that you've truly conquered the gauntlet!"))
	.loadLocstringsFromFile("gauntletgalesLoctable", [] {LOCTABLE_FROMFILE_GAME("gauntletgalesLoctable", "gauntletgalesLabels", "Decor/Text/GauntletGalesLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_3)
	.addRequiredMission(ELevelNames::creeperwoods)
	.selectOrder(4300)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 }))) //D11.PS RETLAMBDA2
	.eventMobs({ em::Illusioner(), em::Evoker() })
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loadingscreen_DLC_GauntletGales.Loadingscreen_DLC_GauntletGales"))
	;

const MissionDef& gauntletgalesDaily = createTrial(ELevelNames::gauntletgalesdaily, ELevelNames::gauntletgales, ELevelVariationType::daily);
const MissionDef& gauntletgalesWeekly = createTrial(ELevelNames::gauntletgalesweekly, ELevelNames::gauntletgales, ELevelVariationType::weekly);
const MissionDef& gauntletgalesSeasonal = createTrial(ELevelNames::gauntletgalesseasonal, ELevelNames::gauntletgales, ELevelVariationType::seasonal);

// D11.CH	=== DLC5 - Oceans ===

// D11.CH	=== DLC5 - Coral Rise ===
const MissionDef& coralrise = create(ELevelNames::coralrise)
	.name(LOCTEXT("coralrise_name", "Coral Rise"))
	.storyTitle(LOCTEXT("coralrise_title", "Danger in the Corals"))
	.storyContents(LOCTEXT("coralrise_contents", "Home to colorful corals and schools of playful tropical fish, Coral Rise is full of life and breathtaking sights. It would be easy to be lulled into a false sense of security by the serene beauty, but the Orb's corruption brews beneath the sandy surface. Hordes of Drowned and Sunken Skeletons lurk in the depths and wait for the currents to bring them fresh foes."))
	.loadLocstringsFromFile("coralriseLoctable", [] {LOCTABLE_FROMFILE_GAME("coralriseLoctable", "coralriseLabels", "Decor/Text/coralriseLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 })))
	.affectors(affector::RuleData({ {EAffectorId::PlayerUnderwater, FAffectorData(1 / 0.8f).SetDefault()}, {EAffectorId::MobUnderwater, FAffectorData(1 / 0.8f).SetDefault()} }))
	.eventMobs({ em::DrownedNecromancer() })
	.nightName(LocTableFromFile::Get("HUDFeatureUILabels.csv", "new_NightName_Oceans"))
	.nightAttackName(LocTableFromFile::Get("HUDFeatureUILabels.csv", "new_NightAttackName_Oceans"))
	.nightMode(ENightModeType::Oceans)
	.selectOrder(6100)
	.metaScorer({
		Required(IsInRange(SubDungeons(), 2, 3)),
		pick::First()
	})
	.requiredDLC(EDLCName::Oceans)
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UMagmaBlockTrigger::StaticClass() }; })
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loadingscreen_DLC_CoralRise.Loadingscreen_DLC_CoralRise"))
	;

const MissionDef& coralriseDaily = createTrial(ELevelNames::coralrisedaily, ELevelNames::coralrise, ELevelVariationType::daily);
const MissionDef& coralriseWeekly = createTrial(ELevelNames::coralriseweekly, ELevelNames::coralrise, ELevelVariationType::weekly);
const MissionDef& coralriseSeasonal = createTrial(ELevelNames::coralriseseasonal, ELevelNames::coralrise, ELevelVariationType::seasonal);

// D11.CH	=== DLC5 - Abyssal Monument ===
const MissionDef& abyssalmonument = create(ELevelNames::abyssalmonument)
	.name(LOCTEXT("abyssalmonument_name", "Abyssal Monument"))
	.storyTitle(LOCTEXT("abyssalmonument_title", "Sunken Sprawl"))
	.storyContents(LOCTEXT("abyssalmonument_contents", "An unbelievable sunken temple sprawls before you, a place lost long ago to time and the changing currents of an indifferent ocean. Forgotten secrets that lie within have been warped by the Orb's corruption, protected by enemies whose weapons and wits have not dulled despite countless eons in the depths. Face the guardian within and claim its treasures for your own."))
	.loadLocstringsFromFile("abyssalmonumentLoctable", [] {LOCTABLE_FROMFILE_GAME("abyssalmonumentLoctable", "abyssalmonumentLabels", "Decor/Text/abyssalmonumentLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 })))
	.affectors(affector::RuleData({ {EAffectorId::PlayerUnderwater, FAffectorData(1 / 0.8f).SetDefault()}, {EAffectorId::MobUnderwater, FAffectorData(1 / 0.8f).SetDefault()} }))
	.eventMobs({ em::ElderGuardian(), em::DrownedNecromancer() })
	.nightName(LocTableFromFile::Get("HUDFeatureUILabels.csv", "new_NightName_Oceans"))
	.nightAttackName(LocTableFromFile::Get("HUDFeatureUILabels.csv", "new_NightAttackName_Oceans"))
	.nightMode(ENightModeType::Oceans)
	.selectOrder(6200)
	.requiredDLC(EDLCName::Oceans)
	.addRequiredMission(ELevelNames::coralrise)
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UMagmaBlockTrigger::StaticClass() }; })
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loadingscreen_DLC_AbyssalMonument.Loadingscreen_DLC_AbyssalMonument"))
	.boss()
	;

const MissionDef& abyssalmonumentDaily = createTrial(ELevelNames::abyssalmonumentdaily, ELevelNames::abyssalmonument, ELevelVariationType::daily);
const MissionDef& abyssalmonumentWeekly = createTrial(ELevelNames::abyssalmonumentweekly, ELevelNames::abyssalmonument, ELevelVariationType::weekly);
const MissionDef& abyssalmonumentSeasonal = createTrial(ELevelNames::abyssalmonumentseasonal, ELevelNames::abyssalmonument, ELevelVariationType::seasonal);

// D11.CH	=== DLC5 - Radiant Ravine ===
const MissionDef& radiantravine = createSecret(ELevelNames::radiantravine, ELevelNames::coralrise)
	.name(LOCTEXT("radiantravine_name", "Radiant Ravine"))
	.storyTitle(LOCTEXT("radiantravine_title", "Undersea Oasis"))
	.storyContents(LOCTEXT("radiantravine_contents", "While most deep ravines are cold and pitch black, the Radiant Ravine is an ever-shifting kaleidoscope of color! Bioluminescent life forms live among the vibrant underwater foliage - but so do legions of Drowned and other hostile creatures. You'll need to stop the Drowned from accidentally destroying the underwater volcano that supports this brilliant oasis, or else this wonderland will fade into total darkness."))
	.loadLocstringsFromFile("radiantravineLoctable", [] {LOCTABLE_FROMFILE_GAME("radiantravineLoctable", "radiantravineLabels", "Decor/Text/radiantravineLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_5)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 })))
	.affectors(affector::RuleData({ {EAffectorId::PlayerUnderwater, FAffectorData(1 / 0.8f).SetDefault()}, {EAffectorId::MobUnderwater, FAffectorData(1 / 0.8f).SetDefault()} }))
	.eventMobs({ em::ElderGuardian(), em::DrownedNecromancer() })
	.nightName(LocTableFromFile::Get("HUDFeatureUILabels.csv", "new_NightName_Oceans"))
	.nightAttackName(LocTableFromFile::Get("HUDFeatureUILabels.csv", "new_NightAttackName_Oceans"))
	.nightMode(ENightModeType::Oceans)
	.selectOrder(6300)
	.victoryDuration(15.0f)
	.requiredDLC(EDLCName::Oceans)
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UMagmaBlockTrigger::StaticClass() }; })
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loadingscreen_DLC_RadiantRavine.Loadingscreen_DLC_RadiantRavine"))
	;

const MissionDef& radiantravineDaily = createTrial(ELevelNames::radiantravinedaily, ELevelNames::radiantravine, ELevelVariationType::daily);
const MissionDef& radiantravineWeekly = createTrial(ELevelNames::radiantravineweekly, ELevelNames::radiantravine, ELevelVariationType::weekly);
const MissionDef& radiantravineSeasonal = createTrial(ELevelNames::radiantravineseasonal, ELevelNames::radiantravine, ELevelVariationType::seasonal);

// D11.MD  === DLC6 - Ender Wilds ===
const MissionDef& enderwilds = create(ELevelNames::enderwilds)
	.name(LOCTEXT("enderwilds_name", "End Wilds"))
	.storyTitle(LOCTEXT("enderwilds_title", "Is This The End?"))
	.storyContents(LOCTEXT("enderwilds_contents", "Far from home and surrounded by the endless Void, the only path ahead takes you deeper into the unknown. Don't mistake the disquieting beauty of the wilderness as peace - the enemies that lurk in the shadows will strike if you let your guard down."))
	.loadLocstringsFromFile("enderwildsLoctable", [] {LOCTABLE_FROMFILE_GAME("enderwildsLoctable", "enderwildsLabels", "Decor/Text/EnderWildsLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_2)
	.nightName(LOCTEXT("NightName_End", "The Void Closes In"))
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors()))
	.eventMobs({ em::Enderman(), em::Endersent().d2() })
	.selectOrder(5500)
	.requiredDLC(EDLCName::TheEnd)
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	.lavaMaterialOverride("LiquidVoid")
	.waterMaterialOverride("MuddyWater")
	.overrideBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidLiquidBlockTrigger::StaticClass() }; })
	.requiredEndGameContent(EEndGameContentType::EndPortal)
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loadingscreen_DLC_EnderWilds.Loadingscreen_DLC_EnderWilds"))
	;

const MissionDef& enderwildsDaily = createTrial(ELevelNames::enderwildsdaily, ELevelNames::enderwilds, ELevelVariationType::daily);
const MissionDef& enderwildsWeekly = createTrial(ELevelNames::enderwildsweekly, ELevelNames::enderwilds, ELevelVariationType::weekly);
const MissionDef& enderwildsSeasonal = createTrial(ELevelNames::enderwildsseasonal, ELevelNames::enderwilds, ELevelVariationType::seasonal);

// D11.MD  === DLC6 - Blighted Citadel ===
const MissionDef& blightedcitadel = create(ELevelNames::blightedcitadel)
	.name(LOCTEXT("blightedcitadel_name", "Broken Citadel"))
	.storyTitle(LOCTEXT("blightedcitadel_title", "Time to End This!"))
	.storyContents(LOCTEXT("blightedcitadel_contents", "You've reached the very heart of the Void and found the last bastion of the enemy's power. It's time to end the tale of the Orb of Dominance and put a stop to this chaos."))
	.loadLocstringsFromFile("blightedcitadelLoctable", [] {LOCTABLE_FROMFILE_GAME("blightedcitadelLoctable", "blightedcitadelLabels", "Decor/Text/BlightedCitadelLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_4)
	.nightName(LOCTEXT("NightName_End", "The Void Closes In"))
	.levelPostProcessConfig(RETLAMBDA2(postprocess::configs::OnlyDoors()))
	.addRequiredMission(ELevelNames::enderwilds)
	.eventMobs({ em::Enderman(), em::Endersent().d2() })
	.selectOrder(5600)
	.endVideo(CreateEndVideoFunctor<UBlightedCitadelOutroVideo>())
	.requiredDLC(EDLCName::TheEnd)
	.lavaMaterialOverride("LiquidVoid")
	.waterMaterialOverride("MuddyWater")
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	.overrideBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidLiquidBlockTrigger::StaticClass() }; })
	.startWithElytra()
	.boss()
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loadingscreen_DLC_BlightedCitadel.Loadingscreen_DLC_BlightedCitadel"))
    ;

const MissionDef& blightedcitadelDaily = createTrial(ELevelNames::blightedcitadeldaily, ELevelNames::blightedcitadel, ELevelVariationType::daily);
const MissionDef& blightedcitadelWeekly = createTrial(ELevelNames::blightedcitadelweekly, ELevelNames::blightedcitadel, ELevelVariationType::weekly);
const MissionDef& blightedcitadelSeasonal = createTrial(ELevelNames::blightedcitadelseasonal, ELevelNames::blightedcitadel, ELevelVariationType::seasonal);

// D11.MD  === DLC6 - The Stronghold ===
const MissionDef& thestronghold = create(ELevelNames::thestronghold)
	.name(LOCTEXT("thestronghold_name", "The Stronghold"))
	.storyTitle(LOCTEXT("thestronghold_title", "A Forgotten Portal"))
	.storyContents(LOCTEXT("thestronghold_contents", "A long-abandoned Stronghold has suddenly become a hub of activity. That can only mean one thing - something ominous has awoken within. Solve the mystery that lies within to find the path forward."))
	.loadLocstringsFromFile("thestrongholdLoctable", [] {LOCTABLE_FROMFILE_GAME("thestrongholdLoctable", "thestrongholdLabels", "Decor/Text/TheStrongholdLabels.csv"); })
	.minimalThreatLevel(EThreatLevel::Threat_3)
	.levelPostProcessConfig(RETLAMBDA2(postprocess::worldfill::providers::FillGradient({ 2, 1, 1, 4, 1, 1, 2, 1, 1, 1 })))
	.eventMobs({ em::Enderman() })
	.selectOrder(3100)
	.showVictoryScreen(false)
	.requiredDLC(EDLCName::TheEnd)
	.additionalBlockTriggers([]() { return TArray<TSubclassOf<UBlockTrigger>> { UVoidBlockTrigger::StaticClass() }; })
	.progressToNextMission(ELevelNames::enderwilds)
	.rewards(game::item::generator::Config([](const auto& state) { return false; }))
	.loadingScreen(FSoftObjectPath("/Game/UI/Materials/LoadingScreens/Loadingscreen_DLC_TheStronghold.Loadingscreen_DLC_TheStronghold"))
	;

const MissionDef& get(ELevelNames level) {
	if (auto mission = getChecked(level)) {
		return *mission;
	}
	return Invalid;
}

const TArray<const MissionDef*> getVariationsBasedOnLevel(ELevelNames level) {
	return getAllMatching(RETLAMBDA(it.levelVariation() != ELevelVariationType::none)).FilterByPredicate(RETLAMBDA(it->variationBasedOnLevel() == level));
}

const MissionDef* getStaticallyDefinedLevelChecked(ELevelNames level) {
	const int index = static_cast<int>(level);

	if (index >= 1 && index < missionDefs.Num()) { // >= 1 Means ELevelNames::Invalid maps to nullptr
		return missionDefs[index].get();
	}
	return nullptr;
}

const MissionDef* getChecked(ELevelNames level) {
	// If we have an override, return that
	const auto overrideDef = missionDefOverrides.find(level);
	if (overrideDef != missionDefOverrides.end()) {
		return overrideDef->second.get();
	}
	return getStaticallyDefinedLevelChecked(level);
}

void ensureLoadAllStringTables() {
	for (auto mission : missions::getAll()) {
		mission->ensureLoadMissionLoc();
	}
}

//Returns current missions defs, with overrides applied
TArray<const MissionDef*> _allDefs() {
	return algo::map_if_tarray(missionDefs, RETLAMBDA(it? getChecked(it->level()) : nullptr), RETLAMBDA(it));
}

TArray<const MissionDef*> _allDefsMatching(const MissionPred& pred) {
	return algo::map_if_tarray(_allDefs(), RETLAMBDA(it), RETLAMBDA(pred(*it)));
}

TArray<const MissionDef*> getAllCurrentlyEnabled() {
	return _allDefsMatching(RETLAMBDA(!it.isDisabled()));
}

TArray<const MissionDef*> getAllCurrentlyEnabledMatching(const MissionPred& pred) {
	return _allDefsMatching(RETLAMBDA(!it.isDisabled() && pred(it)));
}

TArray<const MissionDef*> getAll() {
	return _allDefsMatching(RETLAMBDA(!it.isPermanentlyDisabled()));
}

TArray<const MissionDef*> getAllMatching(const MissionPred& pred) {
	return _allDefsMatching(RETLAMBDA(!it.isPermanentlyDisabled() && pred(it)));
}

bool areMissionDefsInitialized() {
	const auto creeperWoodsIndex = enum_cast(ELevelNames::creeperwoods);
	return missionDefs.IsValidIndex(creeperWoodsIndex) && missionDefs[creeperWoodsIndex];
}

bool shouldPackageAssets(ELevelNames level) {
	if (const auto* missionDef = getChecked(level)) {
		if (auto requiredDlc = missionDef->getRequiredDLC()) {
			if (!dlc::shouldPackageAssets(requiredDlc.GetValue())) {
				return false;
			}
		}
		return !missionDef->isDisabled();
	}
	return false;
}

namespace overrides {

const MutableMissionDef& getOriginal(ELevelNames level) { return static_cast<const MutableMissionDef&>(*getStaticallyDefinedLevelChecked(level)); }
const MutableMissionDef& getCurrent(ELevelNames level) { return static_cast<const MutableMissionDef&>(get(level)); }
void add(Unique<MissionDef> def) { missionDefOverrides[def->level()] = std::move(def); }
void clear() { missionDefOverrides.clear(); }
}

}

//
// Blueprint interface
//

bool UMissionDefs::IsTutorial(ELevelNames level) {
	return missions::get(level).isTutorial();
}

bool UMissionDefs::IsMissionThreatLevelSelectorHidden(ELevelNames LevelName) {
	return missions::get(LevelName).isThreatLevelSelectorHidden();
}

bool UMissionDefs::IsMissionThreatLevelUnlockedByDefault(ELevelNames LevelName) {
	return missions::get(LevelName).isThreatLevelUnlockedByDefault();
}

bool UMissionDefs::IsSecret(ELevelNames LevelName) {
	return missions::get(LevelName).isSecretMission();
}

bool UMissionDefs::IsSalvageEnabled(ELevelNames LevelName) {
	return missions::get(LevelName).isSalvageEnabled();
}

FText UMissionDefs::GetMissionDisplayName(ELevelNames LevelName) {
	return missions::get(LevelName).getNameText();
}

FText UMissionDefs::GetMissionDisplayStoryTitle(ELevelNames LevelName) {
	return missions::get(LevelName).getStoryTitleText();
}

FText UMissionDefs::GetMissionDisplayStoryContents(ELevelNames LevelName) {
	return missions::get(LevelName).getStoryContentsText();
}

UTexture2D* UMissionDefs::GetMissionLoadingScreenTexture(ELevelNames LevelName){
	return missions::get(LevelName).getLoadingScreenTexture();
}

FText UMissionDefs::GetMissionConfirmStartTitle(ELevelNames LevelName) {
	return missions::get(LevelName).getConfirmStartTitleText();
}

FText UMissionDefs::GetMissionConfirmStartWarning(ELevelNames LevelName) {
	return missions::get(LevelName).getConfirmStartWarningText();
}

bool UMissionDefs::GetMissionRequiresStartConfirmation(ELevelNames LevelName) {
	return missions::get(LevelName).requiresOfferings();
}

TSoftObjectPtr<UTexture2D> UMissionDefs::GetMissionLoadingScreenSoftTexture(ELevelNames LevelName) {
	if (auto path = missions::get(LevelName).getLoadingScreenTexturePath()) {
		return TSoftObjectPtr<UTexture2D>(path.GetValue());
	}
	return {};
}


TArray<ELevelNames> UMissionDefs::GetAllNonDisabledMissions() {
	return algo::map_tarray(missions::getAllCurrentlyEnabled(), RETLAMBDA( it->level() ));
}

TArray<ELevelNames> UMissionDefs::GetRequiredMissions(ELevelNames LevelName) {
	return missions::get(LevelName).getRequiredMissions();
}

const FMapColors& UMissionDefs::GetMapColors(ELevelNames LevelName) {
	return missions::get(LevelName).getMapColors();
}

ELevelVariationType UMissionDefs::GetMissionVariationType(ELevelNames LevelName) {
	return missions::get(LevelName).levelVariation();
}

EMissionTheme UMissionDefs::GetMissionUITheme(ELevelNames LevelName) {
	return missions::get(LevelName).theme();
}

bool UMissionDefs::IsVariation(ELevelNames LevelName) {
	const auto variation = GetMissionVariationType(LevelName);
	return variation != ELevelVariationType::Invalid && variation != ELevelVariationType::none;
}

bool UMissionDefs::IsHyperMission(ELevelNames LevelName) {
	return missions::get(LevelName).isHyperMission();
}

bool UMissionDefs::IsTrial(ELevelNames LevelName) {
	return missions::get(LevelName).isTrial();	
}

EExtraChallenge UMissionDefs::GetMissionExtraChallenge(ELevelNames LevelName){	
	return missions::get(LevelName).getExtraChallenge();
}

bool UMissionDefs::GetMissionRequiresOfferings(ELevelNames LevelName) {
	return missions::get(LevelName).requiresOfferings();
}

float getCombinedAverageItemPower(const game::FDifficulty& difficulty) {
	const game::DifficultyStats difficultyStats = game::DifficultyStats(difficulty);
	const FloatRange itemPowerRange = difficultyStats.GetCombinedItemPowerRange();
	const float averageItemRecommendedPower = itemPowerRange.lerp(0.5f);
	return averageItemRecommendedPower;
}

float getHigestAverageItemPower(const game::FDifficulty& difficulty) {
	const game::DifficultyStats difficultyStats = game::DifficultyStats(difficulty);
	const FloatRange dropItemPowerRange = difficultyStats.GetDropItemPowerRange();
	const FloatRange rewardItemPowerRange = difficultyStats.GetRewardItemPowerRange();
	const float averageDropItemRecommendedPower = dropItemPowerRange.lerp(0.5f);
	const float averageRewardItemRecommendedPower = rewardItemPowerRange.lerp(0.5f);
	return FMath::Max(averageDropItemRecommendedPower, averageRewardItemRecommendedPower);
}

float getRecommendationBasingAverageItemPower(const game::FDifficulty& difficulty) {	
	const FloatRange itemPowerRange = FloatRange(getCombinedAverageItemPower(difficulty), getHigestAverageItemPower(difficulty));
	return itemPowerRange.lerp(difficulty.extraChallengeFraction());
}

void UMissionDefs::PopulateRequiredMissions(ELevelNames mission, TSet<ELevelNames>& RequiredMissions) {
	for (auto& required : missions::get(mission).getRequiredMissions()) {
		RequiredMissions.Add(required);
		PopulateRequiredMissions(required, RequiredMissions);
	}
}

TSet<ELevelNames> UMissionDefs::GetRequiredMissionsTree(ELevelNames mission) {
	TSet<ELevelNames> required;
	required.Add(mission);
	PopulateRequiredMissions(mission, required);
	return required;
}

const int UMissionDefs::GetDifficultyRecommendedTotalGearItemPower(const game::FDifficulty& difficulty) {	
	//The first threat level just needs power 1
	if (difficulty.chosen() == difficultyquery::First && difficulty.threatLevel() == threatquery::First && difficulty.extraChallenge() == extrachallengequery::First) {
		return 1;
	}
	
	const float currentAverageItemPower = getRecommendationBasingAverageItemPower(difficulty);

	//Find the previous difficulty which has a change in item power.
	auto previousDifficulty = difficultyquery::getPreviousDifficulty(difficulty);
	while (previousDifficulty.IsSet() && currentAverageItemPower == getRecommendationBasingAverageItemPower(previousDifficulty.GetValue())) {
		previousDifficulty = difficultyquery::getPreviousDifficulty(previousDifficulty.GetValue());
	}

	int recommendedTotalDisplayPower = 0;

	if (previousDifficulty.IsSet()) {		
		const float averagePower = getRecommendationBasingAverageItemPower(previousDifficulty.GetValue());
		
		recommendedTotalDisplayPower = UItemPowerUtil::GetItemPowerDisplayInt(averagePower);
	}

	return recommendedTotalDisplayPower;
}

const int UMissionDefs::GetRecommendedTotalGearItemPower(const FMissionDifficulty& missionDifficulty) {
	return GetDifficultyRecommendedTotalGearItemPower(game::FDifficulty(missionDifficulty));
}

FDifficultyRecommendation UMissionDefs::GetGearPowerDifficultyRecommendation(EExtraChallenge maxExtraChallenge, int DisplayGearPower) {
	//Default to first since we need to have higher power to recommend higher ones.
	FDifficultyRecommendation bestRecommendation = { difficultyquery::First, threatquery::First, extrachallengequery::First, FEndlessStruggle{0}, std::numeric_limits<int>::max() };
	for (int d = enum_cast(difficultyquery::First); d <= enum_cast(difficultyquery::Last); d++) {
		const auto gameDifficulty = static_cast<EGameDifficulty>(d);
		for (int i = enum_cast(threatquery::First); i <= enum_cast(threatquery::Last); i++) {
			const auto threatLevel = static_cast<EThreatLevel>(i);
			const int RecommendedGearPower = GetDifficultyRecommendedTotalGearItemPower(game::FDifficulty(gameDifficulty, threatLevel, extrachallengequery::First));
			const int powerDiff = DisplayGearPower - RecommendedGearPower;
			//Recommend only if you have better gear than recommended
			if (powerDiff >= 0 && powerDiff < bestRecommendation.GearPowerDistance) {
				bestRecommendation = { gameDifficulty, threatLevel, extrachallengequery::First, FEndlessStruggle{0}, powerDiff };
			}
		}
	}

	if (bestRecommendation.RecommendedDifficulty == difficultyquery::Last && bestRecommendation.RecommendedThreatLevel == threatquery::Last){
		for (int es = 0; es <= NumberOfEndlessStruggleLevels; es++) {
			const auto endlessStruggle = FEndlessStruggle{ es };
			const int RecommendedGearPower = GetDifficultyRecommendedTotalGearItemPower(game::FDifficulty(bestRecommendation.RecommendedDifficulty, bestRecommendation.RecommendedThreatLevel, bestRecommendation.RecommendedExtraChallenge, endlessStruggle));
			const int powerDiff = DisplayGearPower - RecommendedGearPower;
			//Recommend only if you have better gear than recommended
			if (powerDiff >= 0 && powerDiff < bestRecommendation.GearPowerDistance) {
				bestRecommendation.RecommendedEndlessStruggle = endlessStruggle;
			}
		}
	}

	if(maxExtraChallenge > extrachallengequery::First){
		//Figure out the recommended extra challenge after because we don't want this to affect the recommended threat level.
		for (int c = enum_cast(extrachallengequery::First); c <= enum_cast(maxExtraChallenge); c++) {
			const auto extraChallenge = static_cast<EExtraChallenge>(c);
			const int RecommendedGearPower = GetDifficultyRecommendedTotalGearItemPower(game::FDifficulty(bestRecommendation.RecommendedDifficulty, bestRecommendation.RecommendedThreatLevel, extraChallenge));
			const int powerDiff = DisplayGearPower - RecommendedGearPower;
			//Recommend only if you have better gear than recommended
			if (powerDiff >= 0 && powerDiff <= bestRecommendation.GearPowerDistance) {
				bestRecommendation.RecommendedExtraChallenge = extraChallenge;
				bestRecommendation.GearPowerDistance = powerDiff;
			}
		}
	}

	return bestRecommendation;
}

FDifficultyRecommendation UMissionDefs::GetGearPowerDifficultyRecommendation(ELevelNames LevelName, int DisplayGearPower) {
	return GetGearPowerDifficultyRecommendation(missions::get(LevelName).getExtraChallenge(), DisplayGearPower);
}

EThreatLevel UMissionDefs::GetLastSelectableThreatLevel(ELevelNames LevelName, EGameDifficulty difficulty)
{
	return threatquery::Last;
}

EThreatLevel UMissionDefs::GetFirstSelectableThreatLevel(ELevelNames LevelName, EGameDifficulty difficulty)
{
	return threatquery::First;
}

TArray<FThreatDescription> UMissionDefs::GetMapThreatLevelDescriptions(const FMissionDifficulty& missionDifficulty, APlayerCharacter* player, int displayGearPower)
{
	if (!missionDifficulty.IsValid()) {
		return {};
	}

	TArray<FThreatDescription> descriptions;	
	const auto difficultyRecommendation = GetGearPowerDifficultyRecommendation(missionDifficulty.mission, displayGearPower);

	const game::DifficultyStats selectedStats(missionDifficulty);
	const game::DifficultyStats recommendedStats{ difficultyRecommendation.CreateDifficulty() };

	const float selectedChallenge = selectedStats.GetMobDamageMultiplier() * selectedStats.GetMobCountMultiplier();
	const float referenceChallenge = recommendedStats.GetMobDamageMultiplier() * recommendedStats.GetMobCountMultiplier();

	const float relativeMobChallenge = selectedChallenge / referenceChallenge;
	if (FMath::IsNearlyEqual(relativeMobChallenge, 1.0f, 0.0001f)) {
		descriptions.Add({EThreatDescriptionType::Default, LOCTEXT("threat_level_normal_difficulty", "Normal difficulty")});
	}
	else {
		descriptions.Add({EThreatDescriptionType::Default, FText::Format(LOCTEXT("threat_level_x_difficulty", "{0}x difficulty"), 
			FText::FromString(valueformat::asSingleDecimalConstant(relativeMobChallenge)))});
	}
	
	if (missions::get(missionDifficulty.mission).areDropsEnabled()) {
		const itemgen::EnvState env {
			game::Settings(selectedStats.GetDifficulty(), 0, missionDifficulty.mission),
			player ? itemgen::looters::FromPlayerCharacter(*player) : itemgen::looters::Unknown()
		};

		const auto dropPowerRange = itemgen::powerrangecalcs::GetDropItemPowerRange(env);
		const auto rewardPowerRange = itemgen::powerrangecalcs::GetRewardItemPowerRange(env);

		if(missions::get(missionDifficulty.mission).getExtraChallenge() == EExtraChallenge::NoExtraChallenge){
			const auto mergedPowerRange = mergeRanges(dropPowerRange, rewardPowerRange);
			descriptions.Add({EThreatDescriptionType::Default, FText::Format(LOCTEXT("threat_level_loot_power", "Gear and artifact power: {0}-{1}"),
				FText::FromString(valueformat::asConstant(UItemPowerUtil::GetItemPowerDisplayInt(mergedPowerRange.min()))),
				FText::FromString(valueformat::asConstant(UItemPowerUtil::GetItemPowerDisplayInt(mergedPowerRange.max())))
			)});
		} else {
			descriptions.Add({EThreatDescriptionType::Default,FText::Format(LOCTEXT("threat_level_drops_power", "Mission drops power: {0}-{1}"),
				FText::FromString(valueformat::asConstant(UItemPowerUtil::GetItemPowerDisplayInt(dropPowerRange.min()))),
				FText::FromString(valueformat::asConstant(UItemPowerUtil::GetItemPowerDisplayInt(dropPowerRange.max())))
			)});
			descriptions.Add({EThreatDescriptionType::Default,FText::Format(LOCTEXT("threat_level_rewards_power", "Completion rewards power: {0}-{1}"),
				FText::FromString(valueformat::asConstant(UItemPowerUtil::GetItemPowerDisplayInt(rewardPowerRange.min()))),
				FText::FromString(valueformat::asConstant(UItemPowerUtil::GetItemPowerDisplayInt(rewardPowerRange.max())))
			)});
		}
	}

	// D11.DB - Endless struggle extra stats.
	if( missionDifficulty.endlessStruggle.Value > 0 ) {
		if( auto gi = Cast<UDungeonsGameInstance>(player->GetGameInstance()) ) {
			if( auto eslib = gi->GetEndlessStruggleLibrary() ) {
				auto selectedES = eslib->GetConfiguration( missionDifficulty.endlessStruggle.Value );
				auto defaultES = FEndlessStruggleConfiguration{};
				if (selectedES.LootQualityBoost != defaultES.LootQualityBoost) {
					descriptions.Add({ EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_loot_quality", "Reroll for rares {0} times"),
						FText::FromString(FString::FromInt(FMath::FloorToInt(selectedES.LootQualityBoost)))) });
				}
				if( selectedES.EnchantedMobChanceMultiplier != defaultES.EnchantedMobChanceMultiplier ) {
					descriptions.Add({EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_mob_enchantment", "{0}x more enchanted mobs"), 
						FText::FromString(valueformat::asSingleDecimalConstant( selectedES.EnchantedMobChanceMultiplier )))});
				}
				if( selectedES.MobDamageMultiplier != defaultES.MobDamageMultiplier ) {
					descriptions.Add({EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_mob_damage", "{0}x mob damage"), 
						FText::FromString(valueformat::asSingleDecimalConstant( selectedES.MobDamageMultiplier )))});
				}
				if( selectedES.MobMaxHealthMultiplier != defaultES.MobMaxHealthMultiplier ) {
					descriptions.Add({EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_mob_health", "{0}x mob health"), 
						FText::FromString(valueformat::asSingleDecimalConstant( selectedES.MobMaxHealthMultiplier )))});
				}
				if( selectedES.MobSpeedMultiplier != defaultES.MobSpeedMultiplier ) {
					descriptions.Add({EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_mob_speed", "{0}x mob speed"), 
						FText::FromString(valueformat::asSingleDecimalConstant( selectedES.MobSpeedMultiplier )))});
				}
				if( selectedES.MobMaxEnduranceMultiplier != defaultES.MobMaxEnduranceMultiplier ) {
					descriptions.Add({EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_mob_stagger_resistance", "{0}x mob stagger resistance"), 
						FText::FromString(valueformat::asSingleDecimalConstant( selectedES.MobMaxEnduranceMultiplier )))});
				}
				if( selectedES.MobResurrectionChance != defaultES.MobResurrectionChance ) {
					descriptions.Add({EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_mob_resurrection", "{0} chance for mobs to revive"), 
						FText::FromString(valueformat::asPercentageChance( selectedES.MobResurrectionChance )))});
				}
				/*
				if (selectedES.MobPushbackMultiplier != defaultES.MobPushbackMultiplier) {
					descriptions.Add({ EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_mob_pushback", "{0}x mob pushback"),
						FText::FromString(valueformat::asSingleDecimalConstant(selectedES.MobPushbackMultiplier))) });
				}
				if (selectedES.PlayerReceiveHealingMultiplier != defaultES.PlayerReceiveHealingMultiplier) {
					descriptions.Add({ EThreatDescriptionType::EndlessStruggle,
						FText::Format(LOCTEXT("endless_struggle_player_healing", "{0}x player healing amount"),
						FText::FromString(valueformat::asSingleDecimalConstant(selectedES.PlayerReceiveHealingMultiplier)))});
				}
				*/
			}
		}
	}

	return descriptions;
}

FText UMissionDefs::GetMissionAsRequirementToolTipText(ELevelNames LevelNameRequired){
	return FText::Format(LOCTEXT("complete_level_x_to_unlock", "Complete {0} to unlock"), GetMissionDisplayName(LevelNameRequired));
}

FText UMissionDefs::GetMissionAsRequirementText(ELevelNames LevelNameRequired) {
	return FText::Format(LOCTEXT("complete_level_x", "Complete {0}"), GetMissionDisplayName(LevelNameRequired));
}

FText UMissionDefs::GetSecretUnlockInMissionAsRequirementText(ELevelNames foundInMission) {
	return FText::Format(LOCTEXT("find_secret_location_in_x", "Find secret location in {0}"), GetMissionDisplayName(foundInMission));
}

FText UMissionDefs::GetMissionTrialText(ELevelNames mission) {
	auto& missionDef = missions::get(mission);
	auto& variationDef = missions::variation::defs().get(missions::get(mission).levelVariation());
	auto& themeText = themes::defs().get(missionDef.theme()).displayText();
	if (themeText.IsSet()) {
		return themeText.GetValue();
	} else if (auto variationDisplay = variationDef.displayText()) {
		return variationDisplay.GetValue();
	} else {
		return FText::GetEmpty();
	}
}

FText UMissionDefs::GetMissionLockedText(ELevelNames mission) {
	auto& missionDef = missions::get(mission);
	auto& lockedThemeText = themes::defs().get(missionDef.theme()).lockedDisplayText();
	return lockedThemeText.Get(FText::GetEmpty());
}

FText UMissionDefs::GetMissionLockedDescription(ELevelNames mission) {
	auto& missionDef = missions::get(mission);
	auto& lockedDescThemeText = themes::defs().get(missionDef.theme()).lockedDescriptionText();
	return lockedDescThemeText.Get(FText::GetEmpty());
}

TArray<FAffectorInfo> UMissionDefs::GetAffectorInfo(ELevelNames mission) {
	affector::Affectors affectors;
	affectors.Set(missions::get(mission).affectors());
	return affectors.GetActiveAffectorsInfo();
}

float UMissionDefs::GetThreatLevelFraction(EThreatLevel threatLevel) {
	return threatquery::toMissionDifficultyFraction(threatLevel);
}

float UMissionDefs::GetDifficultyThreatGlobalThreatFraction(EGameDifficulty difficulty, EThreatLevel threatLevel) {
	return game::FDifficulty(difficulty, threatLevel).globalThreatFraction();
}

float UMissionDefs::GetDisplayGearPowerGlobalThreatFraction(int DisplayGearPower) {
	FDifficultyRecommendation recommendation = GetGearPowerDifficultyRecommendation(EExtraChallenge::NoExtraChallenge, DisplayGearPower);
	auto difficultyFrom = recommendation.CreateDifficulty();	
	if (TOptional<game::FDifficulty> maybedifficultyTo = recommendation.GetNextDifficulty()) {
		const float fromPower = static_cast<float>(GetDifficultyRecommendedTotalGearItemPower(difficultyFrom));
		const float toPower = static_cast<float>(GetDifficultyRecommendedTotalGearItemPower(maybedifficultyTo.GetValue()));
		const float powerRange = fromPower - toPower;
		const float displayGearPowerFloat = static_cast<float>(DisplayGearPower);
		if (powerRange > 0) {
			const float fromGlobalThreat = difficultyFrom.globalThreatFraction();
			const float toGlobalThreat = maybedifficultyTo.GetValue().globalThreatFraction();
			const float powerFraction = (displayGearPowerFloat - fromPower) / powerRange;
			return FloatRange(fromGlobalThreat, toGlobalThreat).clampedLerp(powerFraction);
		}
	}
	return difficultyFrom.globalThreatFraction();
}

ENightModeType UMissionDefs::GetMissionNightModeType(ELevelNames LevelName)
{
	auto mission = missions::getChecked(LevelName);
	return mission ? mission->GetNightModeType() : ENightModeType::Normal;
}

#undef LOCTEXT_NAMESPACE

game::FDifficulty FDifficultyRecommendation::CreateDifficulty() const
{
	return game::FDifficulty(RecommendedDifficulty, RecommendedThreatLevel, RecommendedExtraChallenge, RecommendedEndlessStruggle);
}

TOptional<game::FDifficulty> FDifficultyRecommendation::GetNextDifficulty() const
{
	if (auto nextThreatLevel = threatquery::getNextThreatLevel(RecommendedThreatLevel)) {
		return game::FDifficulty(RecommendedDifficulty, nextThreatLevel.GetValue(), RecommendedExtraChallenge);
	}
	else if (auto nextExtraChallenge = extrachallengequery::getNextExtraChallenge(RecommendedExtraChallenge)) {
		return game::FDifficulty(RecommendedDifficulty, RecommendedThreatLevel, nextExtraChallenge.GetValue());
	}
	else if (auto nextDifficulty = difficultyquery::getNextDifficulty(RecommendedDifficulty)) {
		return game::FDifficulty(nextDifficulty.GetValue(), threatquery::First);
	}
	return {};
}
