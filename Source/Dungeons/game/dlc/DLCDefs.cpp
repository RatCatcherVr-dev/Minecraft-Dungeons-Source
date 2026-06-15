#include "Dungeons.h"
#include "CommonTypes.h"
#include "MutableDLCDef.h"
#include "game/realms/RealmDefs.h"
#include "util/CollectionUtils.h"
#include "util/Algo.h"
#include "util/ConfigFileUtil.h"
#include "Engine/LocalPlayer.h"
#include "OnlineEngineInterface.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/ExternalUI.h"
#include "DLCDefs.h"

namespace dlc {

auto dlcDefs = Util::createDefaultedTArrayOfSize<Unique<MutableDLCDef>>(enum_cast(EDLCName::count));

MutableDLCDef& create(EDLCName dlcIndex) {
	const int index = enum_cast(dlcIndex);
	check(!dlcDefs[index]);
	dlcDefs[index] = make_unique<MutableDLCDef>(dlcIndex);	
	return *dlcDefs[index];
}

#define LOCTEXT_NAMESPACE "DLC"

//
// Realm definitions
//

const DLCDef& Invalid = create(EDLCName::Invalid)
	.disabled()
	;

const DLCDef& TheJungleAwakens = create(EDLCName::TheJungleAwakens)
	.name(LOCTEXT("TheJungleAwakens_name", "Jungle Awakens"))
	.upsellTitle(LOCTEXT("TheJungleAwakens_upsellTitle", "All on the vine"))
	.upsellDescription(LOCTEXT("TheJungleAwakens_upsellDescription", "As new threats roar to life beneath the jungle canopies, join the fight to save these leafy lands."))
	.upsellBulletPoints({
		LOCTEXT("upsellbullet_3newmissions", "3 New Missions"),
		LOCTEXT("upsellbullet_battlenewmobs", "Battle New Mobs"),
		LOCTEXT("upsellbullet_newbossbattle", "New Boss Battle"),
		LOCTEXT("upsellbullet_allnewgear", "All-New Gear")
	})
	.released()
	.addUnlockedByEntitlement("game_dungeons_dlc_1")
	.requiredRealm(ERealmName::IslandsRealm)
	.inspectorBackground(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_jungleawakens_inspector.DLC_jungleawakens_inspector"))
	.inspectorLogo(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_jungleawakens_logo.DLC_jungleawakens_logo"))
#if PLATFORM_WINDOWS	
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	.storePageURL(TEXT("ms-windows-store://pdp/?productid=9NGG3CWJMC7V"))
#else
	.storePageURL(TEXT("https://aka.ms/GetTheJungleAwakens"))
#endif
#elif PLATFORM_XBOXONE
#elif PLATFORM_SWITCH
#elif PLATFORM_PS4
#endif
	.localesRequiringLogoTranslation({
		DungeonsLocale::jaJP
	})
	;

const DLCDef& TheCreepingWinter = create(EDLCName::TheCreepingWinter)
	.name(LOCTEXT("TheCreepingWinter_name", "Creeping Winter"))
	.upsellTitle(LOCTEXT("TheCreepingWinter_upsellTitle", "Fight the frost"))
	.upsellDescription(LOCTEXT("TheCreepingWinter_upsellDescription", "A creeping winter spreads across the land, and it's up to you to defeat the devastating frost."))
	.upsellBulletPoints({
		LOCTEXT("upsellbullet_3newmissions", "3 New Missions"),
		LOCTEXT("upsellbullet_battlenewmobs", "Battle New Mobs"),
		LOCTEXT("upsellbullet_newbossbattle", "New Boss Battle"),
		LOCTEXT("upsellbullet_allnewgear", "All-New Gear")
		})
	.released()
	.addUnlockedByEntitlement("game_dungeons_dlc_2")
	.requiredRealm(ERealmName::IslandsRealm)
	.inspectorBackground(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_creepingwinter_inspector.DLC_creepingwinter_inspector"))
	.inspectorLogo(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_creepingwinter_logo.DLC_creepingwinter_logo"))
#if PLATFORM_WINDOWS	
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	.storePageURL(TEXT("ms-windows-store://pdp/?productid=9P4HCS6S5C2K"))
#else
	.storePageURL(TEXT("https://aka.ms/GetCreepingWinter"))
#endif
#elif PLATFORM_XBOXONE
#elif PLATFORM_SWITCH
#elif PLATFORM_PS4
#endif
	.localesRequiringLogoTranslation({
		DungeonsLocale::jaJP
	})
	;

const DLCDef& Mountains = create(EDLCName::Mountains)
.name(LOCTEXT("TheHowlingPeaks_name", "Howling Peaks"))
.upsellTitle(LOCTEXT("TheHowlingPeaks_upsellTitle", "Trek to the top"))
.upsellDescription(LOCTEXT("TheHowlingPeaks_upsellDescription", "Danger awaits atop mighty peaks, and it will take a hero to stop the brewing storm!"))
.upsellBulletPoints({
	LOCTEXT("upsellbullet_3newmissions", "3 New Missions"),
	LOCTEXT("upsellbullet_battlenewmobs", "Battle New Mobs"),
	LOCTEXT("upsellbullet_newbossbattle", "New Boss Battle"),
	LOCTEXT("upsellbullet_allnewgear", "All-New Gear")
	})
	.released()
	.addUnlockedByEntitlement("game_dungeons_dlc_3")
	.requiredRealm(ERealmName::IslandsRealm)
	.inspectorBackground(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_howlingpeaks_inspector.DLC_howlingpeaks_inspector"))
	.inspectorLogo(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_howlingpeaks_logo.DLC_howlingpeaks_logo"))
#if PLATFORM_WINDOWS	
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	.storePageURL(TEXT("ms-windows-store://pdp/?productid=9N6184JJ7NSG"))
#else
	.storePageURL(TEXT("https://aka.ms/GetHowlingPeaks"))
#endif
#elif PLATFORM_XBOXONE
#elif PLATFORM_SWITCH
#elif PLATFORM_PS4
#endif
	.localesRequiringLogoTranslation({
		DungeonsLocale::jaJP
		})
	;


const DLCDef& Nether = create(EDLCName::Nether)
	.name(LOCTEXT("Nether_name", "Nether"))
	.upsellTitle(LOCTEXT("Nether_upsellTitle", "Playing with Fire"))
	.upsellDescription(LOCTEXT("Nether_upsellDescription", "All the hottest content with new missions, gear, mobs, and cosmetics!"))
	.upsellBulletPoints({
		LOCTEXT("upsellbullet_6newmissions", "6 New Missions"),
		LOCTEXT("upsellbullet_battlenewmobs", "Battle New Mobs"),
		LOCTEXT("upsellbullet_newskinsandpet", "New Skins and Pet"),
		LOCTEXT("upsellbullet_allnewgear", "All-New Gear")
		})
	.released()
	.addUnlockedByEntitlement("game_dungeons_dlc_4")
	.requiredRealm(ERealmName::OtherDimensions)
	.inspectorBackground(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_nether_inspector.DLC_nether_inspector"))
	.inspectorLogo(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_nether_logo.DLC_nether_logo"))
#if PLATFORM_WINDOWS	
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	.storePageURL(TEXT("ms-windows-store://pdp/?productid=9P5JQ1XPRGN6"))
#else
	.storePageURL(TEXT("https://aka.ms/GetFlamesOfTheNether"))
#endif
#endif
	.localesRequiringLogoTranslation({
		DungeonsLocale::jaJP
		})
	;

const DLCDef& Oceans = create(EDLCName::Oceans)
.name(LOCTEXT("Oceans_name", "The Hidden Depths"))
.upsellTitle(LOCTEXT("Oceans_upsellTitle", "Turn the Tide"))
.upsellDescription(LOCTEXT("Oceans_upsellDescription", "Dive into your next adventure with the new Minecraft Dungeons: Hidden Depths DLC!"))
.upsellBulletPoints({
	LOCTEXT("upsellbullet_3newmissions", "3 New Missions"),
	LOCTEXT("upsellbullet_battlenewmobs", "Battle New Mobs"),
	LOCTEXT("upsellbullet_allnewartifacts", "All-New Artifacts"),
	LOCTEXT("upsellbullet_allnewgear", "All-New Gear")
	})
	.released()
	.addUnlockedByEntitlement("game_dungeons_dlc_5")
	.requiredRealm(ERealmName::OtherDimensions)
	.inspectorBackground(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_HiddenDepths_inspector.DLC_HiddenDepths_inspector"))
	.inspectorLogo(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_HiddenDepths_logo.DLC_HiddenDepths_logo"))
#if PLATFORM_WINDOWS	
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	.storePageURL(TEXT("ms-windows-store://pdp/?productid=9P427LFN9KCD"))
#else
	.storePageURL(TEXT("https://aka.ms/GetHiddenDepths"))
#endif
#endif
	.localesRequiringLogoTranslation({
		DungeonsLocale::jaJP
		})
	;

const DLCDef& TheEnd = create(EDLCName::TheEnd)
.name(LOCTEXT("End_name", "Echoing Void"))
.upsellTitle(LOCTEXT("TheEnd_upsellTitle", "TO THE END!"))
.upsellDescription(LOCTEXT("TheEnd_upsellDescription", "You've followed the story of Minecraft Dungeons - now it's time to reach the End!"))
.upsellBulletPoints({
	LOCTEXT("upsellbullet_3newmissions", "3 New Missions"),
	LOCTEXT("upsellbullet_battlenewmobs", "Battle New Mobs"),
	LOCTEXT("upsellbullet_allnewartifacts", "All-New Artifacts"),
	LOCTEXT("upsellbullet_allnewgear", "All-New Gear")
	})
	.released()
	.addUnlockedByEntitlement("game_dungeons_dlc_6")
	.requiredRealm(ERealmName::OtherDimensions)
	.inspectorBackground(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_EchoingVoid_inspector.DLC_EchoingVoid_inspector"))
	.inspectorLogo(FSoftObjectPath("/Game/UI/Materials/MissionSelectMap/dlc/DLC_EchoingVoid_logo.DLC_EchoingVoid_logo"))
#if PLATFORM_WINDOWS	
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	.storePageURL(TEXT("ms-windows-store://pdp/?productid=9PKCNQ57B2JG"))
#else
	.storePageURL(TEXT("https://aka.ms/GetEchoingVoid"))
#endif
#endif
	.localesRequiringLogoTranslation({
		DungeonsLocale::jaJP
		})
	;

const DLCDef& get(EDLCName dlcName) {
	if (auto dlc = getChecked(dlcName)) {
		return *dlc;
	}
	return Invalid;
}

const DLCDef* getChecked(EDLCName dlcName) {
	const int index = static_cast<int>(dlcName);

	if (index >= 1 && index < dlcDefs.Num()) { // >= 1 Means EDLCName::Invalid maps to nullptr
		return dlcDefs[index].get();
	}
	return nullptr;
}


const TArray<EDLCName> getAllEnabled() {
	TArray<EDLCName> all;
	for (auto&& mutableDlcDef : dlc::dlcDefs) {
		DLCDef* dlcDef = mutableDlcDef.get();
		if (dlcDef && !dlcDef->IsDisabled()) {
			all.Add(dlcDef->dlc());
		}
	}
	return all;
}

bool shouldPackageAssets(EDLCName dlcName) {
	if (const auto* dlcDef = getChecked(dlcName)) {
		if (dlcDef->IsDisabled()) {
			return false;
		}
		if (auto requiredRealm = dlcDef->GetRequiredRealm()) {
			if (!realms::shouldPackageAssets(requiredRealm.GetValue())) {
				return false;
			}
		}
		return dlcDef->IsReleased();
	}
	return false;
}

}


//
// Blueprint interface
//

bool UDLCDefs::GetDLCNameReleased(EDLCName dlcName) {
	return dlc::get(dlcName).IsReleased();
}

const FText& UDLCDefs::GetDLCNameText(EDLCName dlcName) {	
	return dlc::get(dlcName).GetName();	
}

const FText& UDLCDefs::GetDLCUpsellTitleText(EDLCName dlcName) {	
	return dlc::get(dlcName).GetUpsellTitleText();	
}

const FText& UDLCDefs::GetDLCUpsellDescriptionText(EDLCName dlcName) {	
	return dlc::get(dlcName).GetUpsellDescriptionText();	
}

TArray<FText> UDLCDefs::GetDLCUpsellBulletPoints(EDLCName dlcName) {	
	return dlc::get(dlcName).GetUpsellBulletPoints();	
}

UTexture2D* UDLCDefs::GetDLCInspectorBackgroundTexture(EDLCName dlcName) {	
	return dlc::get(dlcName).GetInspectorTexture();	
}

UTexture2D* UDLCDefs::GetDLCInspectorLogoTexture(EDLCName dlcName) {	
	return dlc::get(dlcName).GetLogoTexture();	
}

bool UDLCDefs::GetDLCLocaleRequiresLogoTranslation(EDLCName dlcName, DungeonsLocale locale) {	
	return dlc::get(dlcName).DoesLocaleRequireLogoTranslation(locale);	
}

void UDLCDefs::OpenExternalStorePageForDlcName(APlayerController* playerController, FString category, EDLCName dlcName) {


#if PLATFORM_WINDOWS && !defined(STEAM_BUILD)
	if(auto storeURL = dlc::get(dlcName).GetStorePageURL()){
		FPlatformProcess::LaunchURL(*storeURL.GetValue(), NULL, NULL);
	}	
#else
	UWorld* World = playerController ? playerController->GetWorld() : nullptr;
	
#if PLATFORM_SWITCH
	/* Requesting a DLC store page for guest players results in a crash on Switch,
	 * nn::ec::ShowShopApplicationInformation(AppId, Handle) has this precondition:
	 * - The account library is initialized.
	 * Since it is a guest account, it cannot perform purchases.
	 * For this reason, show the store page for the primary local player.
	 */
	if (World)
		playerController = World->GetFirstPlayerController();
#endif

	int LocalUserNum = 0;
	if (playerController)
	{
		ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(playerController->Player);
		if (LocalPlayer)
		{
			LocalUserNum = LocalPlayer->GetControllerId();
		}
	}

	//D11.TT Temporary solution
	FString dlcIniString;
	switch (dlcName)
	{
	case EDLCName::TheJungleAwakens:
		dlcIniString = "JungleDLC";
		break;
	case EDLCName::TheCreepingWinter:
		dlcIniString = "IceDLC";
		break;
	case EDLCName::Mountains:
		dlcIniString = "MountainsDLC";
		break;
	case EDLCName::Nether:
		dlcIniString = "NetherDLC";
		break;
	case EDLCName::Oceans:
		dlcIniString = "OceansDLC";
		break;
	case EDLCName::TheEnd:
		dlcIniString = "TheEndDLC";
		break;
	default:
		check(0);
		dlcIniString = "";
	}

	FString productID = configfile::GetProductId(dlcIniString);
	online::getExternalUIInterface()->ShowStoreUI(World, LocalUserNum, category, productID);

#endif
}

#undef LOCTEXT_NAMESPACE


