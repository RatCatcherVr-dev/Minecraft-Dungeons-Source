#include "Dungeons.h"
#include "MissionThemeDefs.h"
#include "data/MissionThemeRow.h"
#include "data/MissionThemeLibrary.h"
#include <ConstructorHelpers.h>
#include <Engine/DataTable.h>
#include "DungeonsGameInstance.h"
#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>

#define LOCTEXT_NAMESPACE "Theme"
namespace themes {
	auto themeDefs = MissionThemeDefs(enum_cast(EMissionTheme::count), EMissionTheme::Invalid);
	
	const MissionThemeDefs& defs() {
		return themeDefs;
	}

	static auto NormalLockedText = LOCTEXT("Theme_Normal_Locked", "Locked Mission");
	static auto NormalLockedDescText = LOCTEXT("Theme_Normal_Unlock_Desc", "Complete the following to unlock mission:");

	MutableMissionThemeDef& create(EMissionTheme theme, FName rowName) {
		return themeDefs.create(theme).setRowName(rowName).lockedDisplayText(NormalLockedText).lockedDescriptionText(NormalLockedDescText);
	}

	const MissionThemeDef& invalid = create(EMissionTheme::Invalid, "Invalid");
	const MissionThemeDef& story = create(EMissionTheme::Story, "Story");
	const MissionThemeDef& secret = create(EMissionTheme::Secret, "Secret")
		.displayText(LOCTEXT("Theme_Secret", "Secret Mission"));
	const MissionThemeDef& daily = create(EMissionTheme::Daily, "Daily")
		.displayText(LOCTEXT("Theme_Daily", "Daily Mission"));
	const MissionThemeDef& weekly = create(EMissionTheme::Weekly, "Weekly")
		.displayText(LOCTEXT("Theme_Weekly", "Weekly Mission"));
	const MissionThemeDef& seasonal = create(EMissionTheme::Seasonal, "Seasonal")
		.displayText(LOCTEXT("Theme_Seasonal", "Seasonal Mission"));
	const MissionThemeDef& spooky = create(EMissionTheme::Spooky, "Spooky")
		.displayText(LOCTEXT("Theme_Spooky", "Spooky Mission"));
	const MissionThemeDef& jungle = create(EMissionTheme::Jungle, "Jungle")
		.displayText(LOCTEXT("Theme_Jungle", "Jungle Mission"));
	const MissionThemeDef& winter = create(EMissionTheme::Winter, "Winter")
		.displayText(LOCTEXT("Theme_Winter", "Winter Mission"));
	const MissionThemeDef& mountain = create(EMissionTheme::Mountain, "Mountain")
		.displayText(LOCTEXT("Theme_Mountain", "Mountain Mission"));
	const MissionThemeDef& hypermission = create(EMissionTheme::Hypermission, "Hypermission")
		.lockedDisplayText(LOCTEXT("Theme_Hypermission_Locked", "Ancient Hunts Locked"))
		.lockedDescriptionText(LOCTEXT("Theme_Hypermission_Locked_Desc", "Required to unlock Ancient Hunts:"));
	const MissionThemeDef& anniversary = create(EMissionTheme::Anniversary, "Anniversary")
		.displayText(LOCTEXT("Theme_Anniversary", "Anniversary Mission"));
	const MissionThemeDef& spooky2021 = create(EMissionTheme::Spooky2021, "Spooky2021")
		.displayText(LOCTEXT("Theme_Spooky2021", "Spooky Mission"));
}
#undef LOCTEXT_NAMESPACE

const FName& UMissionThemeDefs::getRowName(EMissionTheme theme) {
	return themes::defs().get(theme).rowName();
}

FText UMissionThemeDefs::getDisplayText(EMissionTheme theme) {
	if (auto maybeText = themes::defs().get(theme).displayText()) {
		return maybeText.GetValue();
	}
	return FText::GetEmpty();
}

FText UMissionThemeDefs::getLockedDisplayText(EMissionTheme theme) {
	if (auto maybeText = themes::defs().get(theme).lockedDisplayText()) {
		return maybeText.GetValue();
	}
	return FText::GetEmpty();
}

FText UMissionThemeDefs::getLockedDescriptionText(EMissionTheme theme) {
	if (auto maybeText = themes::defs().get(theme).lockedDescriptionText()) {
		return maybeText.GetValue();
	}
	return FText::GetEmpty();
}

UMissionThemeLibrary* UMissionThemeDefs::getThemeLibrary(UObject* WorldContextObject) {
	check(WorldContextObject && "should not call findRow without a world context object.");
	if(WorldContextObject){
		if (auto* dg = Cast<UDungeonsGameInstance>(WorldContextObject->GetWorld()->GetGameInstance())) {
			return dg->GetMissionThemeLibrary();
		}
	}
	return nullptr;
}

void UMissionThemeDefs::unloadThemeTextures(UObject* WorldContextObject, EMissionTheme theme) {
	if (auto* themelibrary = getThemeLibrary(WorldContextObject)) {
		themelibrary->unloadThemeTextures(theme);
	}	
}

void UMissionThemeDefs::preloadThemeTextures(UObject* WorldContextObject, EMissionTheme theme) {
	if (auto* themelibrary = getThemeLibrary(WorldContextObject)) {
		themelibrary->preloadThemeTextures(theme);
	}	
}
