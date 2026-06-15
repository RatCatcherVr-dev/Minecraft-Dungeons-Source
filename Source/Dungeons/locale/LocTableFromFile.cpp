#include "LocTableFromFile.h"

#include "Paths.h"
#include "StringTableRegistry.h"
#include "Text.h"

// This verbatim loading of files is needed for the scraper to know which files to include in po-exports.
// the scraped string source data is loaded in builds. The folder iteration code below is still kept for easy in-editor workflow!
void LocTableFromFile::LoadCsvs() {
	//
	// UMG COMPATIBLE
	// The CSV loads using the same TableID and Namespace as an UE String Table so no changes are needed to UMG's.
	// * TableID: "/Game/[content subfolder]/[Filename].[Filename]", e.g. "/Game/Decor/Text/AbyssalMonumentLabels.AbyssalMonumentLabels"
	// * Namespace: "[Filename]", e.g. "AbyssalMonumentLabels"
	LOCTABLE_FROMFILE_GAME("/Game/Decor/Text/NetworkUILabels.NetworkUILabels", "NetworkUILabels", "Decor/Text/NetworkUILabels.csv");
	LOCTABLE_FROMFILE_GAME("/Game/Decor/Text/SettingsAccessibilityLabels.SettingsAccessibilityLabels", "SettingsAccessibilityLabels", "Decor/Text/SettingsAccessibilityLabels.csv");
	LOCTABLE_FROMFILE_GAME("/Game/Content_DLC4/Text/DLC4UILabels.DLC4UILabels", "DLC4UILabels", "Content_DLC4/Text/DLC4UILabels.csv");
	LOCTABLE_FROMFILE_GAME("/Game/Content_DLC4/Text/GoldLabels.GoldLabels", "GoldLabels", "Content_DLC4/Text/GoldLabels.csv");
	LOCTABLE_FROMFILE_GAME("/Game/Content_DLC4/Text/HyperMissionLabels.HyperMissionLabels", "HyperMissionLabels", "Content_DLC4/Text/HyperMissionLabels.csv");
	LOCTABLE_FROMFILE_GAME("/Game/Content_DLC4/Text/AncientLabels.AncientLabels", "AncientLabels", "Content_DLC4/Text/AncientLabels.csv");

	//
	// NEW FILES
	// The CSV loads using the a short TableID with the ".csv" postfix, and Namespace as the file path so it's easy to find where the text source.
	// * TableID: "[Filename].csv", e.g. "AbyssalMonumentLabels.csv"
	// * Namespace: "[Full file path]", e.g. "Decor/Text/AbyssalMonumentLabels.csv"
	LOCTABLE_FROMFILE_GAME("CosmeticsLabels.csv", "Decor/Text/CosmeticsLabels.csv", "Decor/Text/CosmeticsLabels.csv");
	LOCTABLE_FROMFILE_GAME("InventoryHUDLabels.csv", "Decor/Text/InventoryHUDLabels.csv", "Decor/Text/InventoryHUDLabels.csv");
	LOCTABLE_FROMFILE_GAME("CinematicMenuLabels.csv", "Content_DLC6/Decor/Text/CinematicMenuLabels.csv", "Content_DLC6/Decor/Text/CinematicMenuLabels.csv");
	LOCTABLE_FROMFILE_GAME("DLC6MerchantLabels.csv", "Content_DLC6/Decor/Text/DLC6MerchantLabels.csv", "Content_DLC6/Decor/Text/DLC6MerchantLabels.csv");
	LOCTABLE_FROMFILE_GAME("DLC6UIHints.csv", "Content_DLC6/Decor/Text/DLC6UIHints.csv", "Content_DLC6/Decor/Text/DLC6UIHints.csv");
	LOCTABLE_FROMFILE_GAME("DLC6UILabels.csv", "Content_DLC6/Decor/Text/DLC6UILabels.csv", "Content_DLC6/Decor/Text/DLC6UILabels.csv");
	LOCTABLE_FROMFILE_GAME("NewsFeedText.csv", "Content_DLC6/Decor/Text/NewsFeedText.csv", "Content_DLC6/Decor/Text/NewsFeedText.csv");
	LOCTABLE_FROMFILE_GAME("HUDFeatureUILabels.csv", "Content_DLC6/Decor/Text/HUDFeatureUILabels.csv", "Content_DLC6/Decor/Text/HUDFeatureUILabels.csv");
	LOCTABLE_FROMFILE_GAME("Season1Labels.csv", "Content_Season1/Decor/Text/Season1Labels.csv", "Content_Season1/Decor/Text/Season1Labels.csv");
	LOCTABLE_FROMFILE_GAME("ConnectionStatusLabels.csv", "Content_Spooky2021/Decor/Text/ConnectionStatusLabels.csv", "Content_Spooky2021/Decor/Text/ConnectionStatusLabels.csv");
	LOCTABLE_FROMFILE_GAME("MapNewsLabels.csv", "Content_Spooky2021/Decor/Text/MapNewsLabels.csv", "Content_Spooky2021/Decor/Text/MapNewsLabels.csv");
	LOCTABLE_FROMFILE_GAME("SpookyFallEventLabels.csv", "Content_Spooky2021/Decor/Text/SpookyFallEventLabels.csv", "Content_Spooky2021/Decor/Text/SpookyFallEventLabels.csv");
}

FText LocTableFromFile::Get(const FName& InTableId, const FString& InKey) {
	return FText::FromStringTable(InTableId, InKey);
}
