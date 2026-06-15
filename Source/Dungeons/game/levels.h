#pragma once

#include "util/EnumUtil.h"
#include "common_header.h"

UENUM(BlueprintType)
enum class ELevelNames : uint8 {
	Invalid,

	squidcoast UMETA(DisplayName="Squid Coast"),
	creeperwoods UMETA(DisplayName = "Creeper Woods"),
	pumpkinpastures UMETA(DisplayName = "Pumpkin Pastures"),
	soggyswamp UMETA(DisplayName = "Soggy Swamp"),
	mooncorecaverns UMETA(DisplayName = "Mooncore Caverns"),
	fieryforge UMETA(DisplayName = "Fiery Forge"),
	deserttemple UMETA(DisplayName = "Desert Temple"),
	slimysewers UMETA(DisplayName = "Slimy Sewers"),
	highblockhalls UMETA(DisplayName = "Highblock Halls"),
	obsidianpinnacle UMETA(DisplayName = "Obsidian Pinnacle"),
	cacticanyon UMETA(DisplayName = "Cacti Canyon"),

	// daylies:
	// squidcoastdaily UMETA(DisplayName = "Squid Coast Daily"),
	creeperwoodsdaily UMETA(DisplayName = "Creeper Woods Daily"),
	creeperwoodsweekly UMETA(DisplayName = "Creeper Woods Weekly"),
	creeperwoodsseasonal UMETA(DisplayName = "Creeper Woods Seasonal"),

	pumpkinpasturesdaily UMETA(DisplayName = "Pumpkin Pastures Daily"),
	pumpkinpasturesweekly UMETA(DisplayName = "Pumpkin Pastures Weekly"),
	pumpkinpasturesseasonal UMETA(DisplayName = "Pumpkin Pastures Seasonal"),

	soggyswampdaily UMETA(DisplayName = "Soggy Swamp Daily"),
	soggyswampweekly UMETA(DisplayName = "Soggy Swamp Weekly"),
	soggyswampseasonal UMETA(DisplayName = "Soggy Swamp Seasonal"),

	mooncorecavernsdaily UMETA(DisplayName = "Mooncore Caverns Daily"),
	mooncorecavernsweekly UMETA(DisplayName = "Mooncore Caverns Weekly"),
	mooncorecavernsseasonal UMETA(DisplayName = "Mooncore Caverns Seasonal"),

	fieryforgedaily UMETA(DisplayName = "Fiery Forge Daily"),
	fieryforgeweekly UMETA(DisplayName = "Fiery Forge Weekly"),
	fieryforgeseasonal UMETA(DisplayName = "Fiery Forge Seasonal"),

	deserttempledaily UMETA(DisplayName = "Desert Temple Daily"),
	deserttempleweekly UMETA(DisplayName = "Desert Temple Weekly"),
	deserttempleseasonal UMETA(DisplayName = "Desert Temple Seasonal"),

	slimysewersdaily UMETA(DisplayName = "Slimy Sewers Daily"),
	slimysewersweekly UMETA(DisplayName = "Slimy Sewers Weekly"),
	slimysewersseasonal UMETA(DisplayName = "Slimy Sewers Seasonal"),

	highblockhallsdaily UMETA(DisplayName = "Highblock Halls Daily"),
	highblockhallsweekly UMETA(DisplayName = "Highblock Halls Weekly"),
	highblockhallsseasonal UMETA(DisplayName = "Highblock Halls Seasonal"),

	obsidianpinnacledaily UMETA(DisplayName = "Obsidian Pinnacle Daily"),
	obsidianpinnacleweekly UMETA(DisplayName = "Obsidian Pinnacle Weekly"),
	obsidianpinnacleseasonal UMETA(DisplayName = "Obsidian Pinnacle Seasonal"),

	cacticanyondaily UMETA(DisplayName = "Cacti Canyon Daily"),
	cacticanyonweekly UMETA(DisplayName = "Cacti Canyon Weekly"),
	cacticanyonseasonal UMETA(DisplayName = "Cacti Canyon Seasonal"),


	// bonus
	creepycrypt UMETA(DisplayName = "Creepy Crypt"),
	soggycave UMETA(DisplayName = "Soggy Cave"),
	underhalls UMETA(DisplayName = "Underhalls"),
	archhaven UMETA(DisplayName = "Arch Haven"),
	lowertemple UMETA(DisplayName = "Lower Temple"),
	mooshroomisland UMETA(DisplayName = "???"),
	woodlandmansion UMETA(DisplayName = "Woodland Mansion"),
	spidercave UMETA(DisplayName = "Spider Cave"),

	// D11 (dlc1, dlc2)
	dingyjungle UMETA(DisplayName = "Dingy Jungle"),
	overgrowntemple UMETA(DisplayName = "Overgrown Temple"),
	bamboobluff UMETA(DisplayName = "Bamboo Bluff"),
	frozenfjord UMETA(DisplayName = "Frozen Fjord"),
	lonelyfortress UMETA(DisplayName = "Lonely Fortress"),
	lostsettlement UMETA(DisplayName = "Lost Settlement"),

	// bonus trials
	creepycryptdaily UMETA(DisplayName = "Creepy Crypt Daily"),
	creepycryptweekly UMETA(DisplayName = "Creepy Crypt Weekly"),
	creepycryptseasonal UMETA(DisplayName = "Creepy Crypt Seasonal"),

	soggycavedaily UMETA(DisplayName = "Soggy Cave Daily"),
	soggycaveweekly UMETA(DisplayName = "Soggy Cave Weekly"),
	soggycaveseasonal UMETA(DisplayName = "Soggy Cave Seasonal"),

	underhallsdaily UMETA(DisplayName = "Underhalls Daily"),
	underhallsweekly UMETA(DisplayName = "Underhalls Weekly"),
	underhallsseasonal UMETA(DisplayName = "Underhalls Seasonal"),

	archhavendaily UMETA(DisplayName = "Arch Haven Daily"),
	archhavenweekly UMETA(DisplayName = "Arch Haven Weekly"),
	archhavenseasonal UMETA(DisplayName = "Arch Haven Seasonal"),

	lowertempledaily UMETA(DisplayName = "Lower Temple Daily"),
	lowertempleweekly UMETA(DisplayName = "Lower Temple Weekly"),
	lowertempleseasonal UMETA(DisplayName = "Lower Temple Seasonal"),

	mooshroomislanddaily UMETA(DisplayName = "??? Daily"),
	mooshroomislandweekly UMETA(DisplayName = "??? Weekly"),
	mooshroomislandseasonal UMETA(DisplayName = "??? Seasonal"),


	// D11 (dlc1, dlc2) trials
	dingyjungledaily UMETA(DisplayName = "Dingy Jungle Daily"),
	dingyjungleweekly UMETA(DisplayName = "Dingy Jungle Weekly"),
	dingyjungleseasonal UMETA(DisplayName = "Dingy Jungle Seasonal"),

	overgrowntempledaily UMETA(DisplayName = "Overgrown Temple Daily"),
	overgrowntempleweekly UMETA(DisplayName = "Overgrown Temple Weekly"),
	overgrowntempleseasonal UMETA(DisplayName = "Overgrown Temple Seasonal"),

	bamboobluffdaily UMETA(DisplayName = "Bamboo Bluff Daily"),
	bamboobluffweekly UMETA(DisplayName = "Bamboo Bluff Weekly"),
	bamboobluffseasonal UMETA(DisplayName = "Bamboo Bluff Seasonal"),

	frozenfjorddaily UMETA(DisplayName = "Frozen Fjord Daily"),
	frozenfjordweekly UMETA(DisplayName = "Frozen Fjord Weekly"),
	frozenfjordseasonal UMETA(DisplayName = "Frozen Fjord Seasonal"),

	lonelyfortressdaily UMETA(DisplayName = "Lonely Fortress Daily"),
	lonelyfortressweekly UMETA(DisplayName = "Lonely Fortress Weekly"),
	lonelyfortressseasonal UMETA(DisplayName = "Lonely Fortress Seasonal"),

	lostsettlementdaily UMETA(DisplayName = "Lost Settlement Daily"),
	lostsettlementweekly UMETA(DisplayName = "Lost Settlement Weekly"),
	lostsettlementseasonal UMETA(DisplayName = "Lost Settlement Seasonal"),

	// DLC - Mojang
	crimsonforest UMETA(DisplayName = "Crimson Forest"),
	basaltdeltas UMETA(DisplayName = "Basalt Deltas"),
	warpedforest UMETA(DisplayName = "Warped Forest"),
	soulsandvalley UMETA(DisplayName = "Soulsand Valley"),
	netherwastes UMETA(DisplayName = "Endless Wastes"),
	netherfortress UMETA(DisplayName = "Blazing Fortress"),

	netherhypermission UMETA(DisplayName = "Nether HyperMission"),

	// DLC - Mojang Trials
	crimsonforestdaily UMETA(DisplayName = "Crimson Forest Daily"),
	crimsonforestweekly UMETA(DisplayName = "Crimson Forest Weekly"),
	crimsonforestseasonal UMETA(DisplayName = "Crimson Forest Seasonal"),

	basaltdeltasdaily UMETA(DisplayName = "Basalt Deltas Daily"),
	basaltdeltasweekly UMETA(DisplayName = "Basalt Deltas Weekly"),
	basaltdeltasseasonal UMETA(DisplayName = "Basalt Deltas Seasonal"),

	warpedforestdaily UMETA(DisplayName = "Warped Forest Daily"),
	warpedforestweekly UMETA(DisplayName = "Warped Forest Weekly"),
	warpedforestseasonal UMETA(DisplayName = "Warped Forest Seasonal"),

	soulsandvalleydaily UMETA(DisplayName = "Soulsand Valley Daily"),
	soulsandvalleyweekly UMETA(DisplayName = "Soulsand Valley Weekly"),
	soulsandvalleyseasonal UMETA(DisplayName = "Soulsand Valley Seasonal"),

	netherwastesdaily UMETA(DisplayName = "Endless Wastes Daily"),
	netherwastesweekly UMETA(DisplayName = "Endless Wastes Weekly"),
	netherwastesseasonal UMETA(DisplayName = "Endless Wastes Seasonal"),

	netherfortressdaily UMETA(DisplayName = "Blazing Fortress Daily"),
	netherfortressweekly UMETA(DisplayName = "Blazing Fortress Weekly"),
	netherfortressseasonal UMETA(DisplayName = "Blazing Fortress Seasonal"),

	// D11 DLC3
	windsweptpeaks UMETA(DisplayName = "Windswept Peaks"),
	galesanctum UMETA(DisplayName = "Gale Sanctum"),
	endlessrampart UMETA(DisplayName = "Endless Rampart"),
	gauntletgales UMETA(DisplayName = "Gauntlet of Gales"),

	// D11 DLC3 Trials
	windsweptpeaksdaily UMETA(DisplayName = "Windswept Peaks Daily"),
	windsweptpeaksweekly UMETA(DisplayName = "Windswept Peaks Weekly"),
	windsweptpeaksseasonal UMETA(DisplayName = "Windswept Peaks Seasonal"),

	galesanctumdaily UMETA(DisplayName = "Gale Sanctum Daily"),
	galesanctumweekly UMETA(DisplayName = "Gale Sanctum Weekly"),
	galesanctumseasonal UMETA(DisplayName = "Gale Sanctum Seasonal"),

	endlessrampartdaily UMETA(DisplayName = "Endless Rampart Daily"),
	endlessrampartweekly UMETA(DisplayName = "Endless Rampart Weekly"),
	endlessrampartseasonal UMETA(DisplayName = "Endless Rampart Seasonal"),

	gauntletgalesdaily UMETA(DisplayName = "Gauntlet of Gales Daily"),
	gauntletgalesweekly UMETA(DisplayName = "Gauntlet of Gales Weekly"),
	gauntletgalesseasonal UMETA(DisplayName = "Gauntlet of Gales Seasonal"),

	// D11 DLC5
	coralrise UMETA(DisplayName = "Coral Rise"),
	abyssalmonument UMETA(DisplayName = "The Abyssal Monument"),
	radiantravine UMETA(DisplayName = "Radiant Ravine"),

	// D11 DLC5 Trials
	coralrisedaily UMETA(DisplayName = "Coral Rise Daily"),
	coralriseweekly UMETA(DisplayName = "Coral Rise Weekly"),
	coralriseseasonal UMETA(DisplayName = "Coral Rise Seasonal"),

	abyssalmonumentdaily UMETA(DisplayName = "The Abyssal Monument Daily"),
	abyssalmonumentweekly UMETA(DisplayName = "The Abyssal Monument Weekly"),
	abyssalmonumentseasonal UMETA(DisplayName = "The Abyssal Monument Seasonal"),
		
	radiantravinedaily UMETA(DisplayName = "Radiant Ravine Daily"),
	radiantravineweekly UMETA(DisplayName = "Radiant Ravine Weekly"),
	radiantravineseasonal UMETA(DisplayName = "Radiant Ravine Seasonal"),

	// D11 DLC6
	enderwilds UMETA(DisplayName = "End Wilds"),
	blightedcitadel UMETA(DisplayName = "Broken Citadel"),
	thestronghold UMETA(DisplayName = "The Stronghold"),

	// D11 DLC6 Trials
	enderwildsdaily UMETA(DisplayName = "End Wilds Daily"),
	enderwildsweekly UMETA(DisplayName = "End Wilds Weekly"),
	enderwildsseasonal UMETA(DisplayName = "End Wilds Seasonal"),

	blightedcitadeldaily UMETA(DisplayName = "Broken Citadel Daily"),
	blightedcitadelweekly UMETA(DisplayName = "Broken Citadel Weekly"),
	blightedcitadelseasonal UMETA(DisplayName = "Broken Citadel Seasonal"),

	// Test
	specialtileshub UMETA(DisplayName = "Special Tiles Hub"),
	hm_hub UMETA(DisplayName = "HM Hub"),

	// END:
	count
};
ENUM_NAME(ELevelNames);

UENUM(NotBlueprintType)
enum class ELobbyNames : uint8 {
	Invalid,

	lobby UMETA(DisplayName = "Lobby")
};
ENUM_NAME(ELobbyNames);

namespace level {

	ELevelNames fromString(const FString& missionId);
	ELevelNames fromString(const std::string& missionId);
}
