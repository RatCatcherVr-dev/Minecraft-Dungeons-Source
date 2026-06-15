#include "Dungeons.h"
#include "MerchantDefs.h"
#include "type/VillageMerchant.h"
#include "type/MysteryMerchant.h"
#include "type/BlacksmithMerchant.h"
#include "type/GiftWrapperMerchant.h"
#include "type/QuestGiverMerchant.h"
#include "type/LuxuryMerchant.h"
#include "type/PiglinMerchant.h"
#include "type/HyperMissionMerchant.h"
#include "type/EnchantmentMerchant.h"
#include "game/levels.h"
#include "locale/LocTableFromFile.h"
#include "type/AdventureHubMerchant.h"

#define LOCTEXT_NAMESPACE "Merchants"

UVillageMerchantDef::UVillageMerchantDef() {
	DisplayName = LOCTEXT("VillageMerchant", "Village Merchant");
	MerchantClass = AVillageMerchant::StaticClass();
	UnlockProgressKey = TEXT("shopkeeper_unlocked");
	LegacyUnlockMission = ELevelNames::creeperwoods;
	GenerationLevelProgressKeys = {
		TEXT("shopkeeper_lvl1"),
		TEXT("shopkeeper_lvl2"),
		TEXT("shopkeeper_lvl3"),
		TEXT("shopkeeper_lvl4"),
		TEXT("shopkeeper_lvl5"),
	};
}

ULuxuryMerchantDef::ULuxuryMerchantDef() {
	DisplayName = LOCTEXT("LuxuryMerchant", "Luxury Merchant");
	MerchantClass = ALuxuryMerchant::StaticClass();
	UnlockProgressKey = TEXT("luxurymerchant_unlocked");
	LegacyUnlockMission = ELevelNames::cacticanyon;
	GenerationLevelProgressKeys = {
		TEXT("luxurymerchant_lvl1"),
		TEXT("luxurymerchant_lvl2"),
		TEXT("luxurymerchant_lvl3"),		
	};
}

UMysteryMerchantDef::UMysteryMerchantDef() {
	DisplayName = LOCTEXT("MysteryMerchant", "Mystery Merchant");
	MerchantClass = AMysteryMerchant::StaticClass();
	UnlockProgressKey = TEXT("mysterymerchant_unlocked");
	LegacyUnlockMission = ELevelNames::pumpkinpastures;
	GenerationLevelProgressKeys = {
		TEXT("mysterymerchant_lvl1"),
		TEXT("mysterymerchant_lvl2"),
		TEXT("mysterymerchant_lvl3"),
		TEXT("mysterymerchant_lvl4"),
		TEXT("mysterymerchant_lvl5"),
	};
}

UBlacksmithMerchantDef::UBlacksmithMerchantDef() {
	DisplayName = LOCTEXT("BlacksmithMerchant", "Blacksmith");
	MerchantClass = ABlacksmithMerchant::StaticClass();
	UnlockProgressKey = TEXT("blacksmith_unlocked");
	LegacyUnlockMission = ELevelNames::mooncorecaverns;
	GenerationLevelProgressKeys = {
		TEXT("blacksmith_lvl1"),
		TEXT("blacksmith_lvl2"),
		TEXT("blacksmith_lvl3"),
	};
}

UGiftWrapperMerchantDef::UGiftWrapperMerchantDef() {
	DisplayName = LOCTEXT("Giftwrapper", "Gift Wrapper");
	MerchantClass = AGiftWrapperMerchant::StaticClass();
	UnlockProgressKey = TEXT("giftwrapper_unlocked");
	LegacyUnlockMission = ELevelNames::soggyswamp;
	GenerationLevelProgressKeys = {
		//TEXT("giftwrapper_lvl1"), Gift wrapper only has 1 level now, but geo still has 3 levels.
		//TEXT("giftwrapper_lvl2"), Only use level 3 for visualization.
		TEXT("giftwrapper_lvl3"),
	};
}

UPiglinMerchantDef::UPiglinMerchantDef() {
	DisplayName = LOCTEXT("PiglinMerchant", "Piglin Merchant");
	MerchantClass = APiglinMerchant::StaticClass();
	UnlockProgressKey = TEXT("piglinmerchant_unlocked");
	GenerationLevelProgressKeys = {
		TEXT("piglinmerchant_lvl1"),
		TEXT("piglinmerchant_lvl2"),
		TEXT("piglinmerchant_lvl3"),
	};
}

UEnchantmentMerchantDef::UEnchantmentMerchantDef() {
	DisplayName = LocTableFromFile::Get("DLC6MerchantLabels.csv", "merchant_enchantment_display_name");
	MerchantClass = AEnchantmentMerchant::StaticClass();
}

UQuestGiverMerchantDef::UQuestGiverMerchantDef() {
	DisplayName = LOCTEXT("Retired Adventurer", "Retired Adventurer");
	MerchantClass = AQuestGiverMerchant::StaticClass();
	UnlockProgressKey = TEXT("questgiver_unlocked");
}

UHyperMissionMerchantDef::UHyperMissionMerchantDef() {
	DisplayName = LOCTEXT("AncientHunt", "Ancient Hunt");
	MerchantClass = AHyperMissionMerchant::StaticClass();
	UnlockProgressKey = TEXT("hypermission_unlocked");
}

UAdventureHubMerchantDef::UAdventureHubMerchantDef() {
	DisplayName = LOCTEXT("AdventureHub", "Adventure Hub");
	MerchantClass = AAdventureHubMerchant::StaticClass();
	UnlockProgressKey = TEXT("adventurehub_unlocked");
}

#undef LOCTEXT_NAMESPACE

// D11.AH - WARNING - Please be careful changing this to an initializer list as it breaks
//                    the Nintendo switch optimization compiler, hence the changes.
TArray<TSubclassOf<UMerchantDef>> merchantdefs::getAllEnabledClasses() {
	TArray<TSubclassOf<UMerchantDef>> R;
	R.Add(UVillageMerchantDef::StaticClass());
	R.Add(ULuxuryMerchantDef::StaticClass());
	R.Add(UMysteryMerchantDef::StaticClass());
	R.Add(UBlacksmithMerchantDef::StaticClass());
	R.Add(UGiftWrapperMerchantDef::StaticClass());
	R.Add(UPiglinMerchantDef::StaticClass());
	R.Add(UHyperMissionMerchantDef::StaticClass());
	R.Add(UAdventureHubMerchantDef::StaticClass());
	return R;
}

// D11.SSN - made due to the duplicated Piglin Merchant & the HyperMission merchant present in above function.
TArray<TSubclassOf<UMerchantDef>> merchantdefs::getAllEnabledMerchants() {
	TArray<TSubclassOf<UMerchantDef>> R;
	R.Add(UVillageMerchantDef::StaticClass());
	R.Add(ULuxuryMerchantDef::StaticClass());
	R.Add(UMysteryMerchantDef::StaticClass());
	R.Add(UBlacksmithMerchantDef::StaticClass());
	R.Add(UGiftWrapperMerchantDef::StaticClass());
	R.Add(UPiglinMerchantDef::StaticClass());
	return R;
}

//#pragma clang optimize on
