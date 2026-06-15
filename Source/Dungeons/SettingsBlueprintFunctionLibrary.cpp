#include "SettingsBlueprintFunctionLibrary.h"
#include "DungeonsGameInstance.h"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "save/GlobalSaveData.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "PlayfabServices.h"
#include <Themida/Anticheat.hpp>
#include "game/dlc/DLCDefs.h"
#include "MediaSource.h"

bool USettingsBlueprintFunctionLibrary::bUnlockAllDLC = false;

namespace {
	bool GetSettingJsonFromSave(FString const &varName, const APlayerControllerBase *PlayerController, Json::Value &outValue)
	{
		if (PlayerController)
		{
			auto gameInstance = PlayerController->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
			auto localPlayer = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer());
			if (localPlayer) {
				int LocalUserNum = localPlayer->GetSystemUserId();
				if (auto* save = gameInstance->EditGlobalSaveState(LocalUserNum))
				{
					Json::Value& node = save->GetSettingsNode();
					outValue = node[TCHAR_TO_UTF8(*varName)];
					if (!outValue.isNull())
						return true;
				}
			}
			else {
				UE_LOG(LogDungeons,
					Warning,
					TEXT("Local player failed to initialize when trying to get setting %s from save. This happens when the server tries to get settings for a remote playercontroller during level transition"),
					*varName
				);
			}
		}
		return false;
	}

	bool SetSettingJsonByName(FString const &varName, Json::Value newValue, APlayerControllerBase *PlayerControllerBase)
	{
		if (PlayerControllerBase)
		{
			auto gameInstance = PlayerControllerBase->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
			int LocalUserNum = Cast<UDungeonsLocalPlayer>(PlayerControllerBase->GetLocalPlayer())->GetSystemUserId();
			if (auto* save = gameInstance->EditGlobalSaveState(LocalUserNum))
			{
				Json::Value& node = save->GetSettingsNode();
				node[TCHAR_TO_UTF8(*varName)] = newValue;
				return true;
			}
		}
		return false;
	}
}

bool USettingsBlueprintFunctionLibrary::GetSettingFromSave(FString varName, const APlayerControllerBase* PlayerController, int& outInt)
{
	Json::Value jsonValue{};
	bool valueFound = GetSettingJsonFromSave(varName, PlayerController, jsonValue);
	if (valueFound)
		outInt = jsonValue.asInt();
	return valueFound;
}

bool USettingsBlueprintFunctionLibrary::SetSettingByName(FString varName, int newValue, APlayerControllerBase* PlayerControllerBase)
{
	return SetSettingJsonByName(varName, Json::Value(newValue), PlayerControllerBase);
}

bool USettingsBlueprintFunctionLibrary::GetSettingFromSaveInt64(FString varName, const APlayerControllerBase* PlayerController, int64& outInt)
{
	Json::Value jsonValue{};
	bool valueFound = GetSettingJsonFromSave(varName, PlayerController, jsonValue);
	if (valueFound)
		outInt = jsonValue.asInt64();
	return valueFound;
}

bool USettingsBlueprintFunctionLibrary::SetSettingByNameInt64(FString varName, int64 newValue, APlayerControllerBase* PlayerControllerBase)
{
	return SetSettingJsonByName(varName, Json::Value(newValue), PlayerControllerBase);
}

bool USettingsBlueprintFunctionLibrary::SetXblActive(bool newValue, APlayerControllerBase* PlayerControllerBase)
{
	return online::SetXblActive(newValue, PlayerControllerBase);
}

bool USettingsBlueprintFunctionLibrary::GetIntByName(UObject* target, FName varName, int& outInt)
{
	if (target)
	{			
		if (auto* intProp = FindField<UIntProperty>(target->GetClass(), varName))
		{
			outInt = intProp->GetPropertyValue_InContainer(target);
			return true;
		}
	}
	return false;
}

bool USettingsBlueprintFunctionLibrary::SetIntByName(UObject* target, FName varName, int32 newValue, int32& outInt)
{
	if (target)
	{
		if (auto* intProp = FindField<UIntProperty>(target->GetClass(), varName))
		{
			intProp->SetPropertyValue_InContainer(target, newValue);
			outInt = intProp->GetPropertyValue_InContainer(target);

			return true;
		}
	}
	return false;
}

void USettingsBlueprintFunctionLibrary::SaveGame(UObject* target, APlayerControllerBase* PlayerController)
{
	if (PlayerController)
	{
		auto gameInstance = PlayerController->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
		int LocalUserNum = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer())->GetSystemUserId();
		if (auto* save = gameInstance->EditGlobalSaveState(LocalUserNum))
		{
			Json::Value& node = save->GetSettingsNode();

			for (TFieldIterator<UProperty> PropIt(target->GetClass()); PropIt; ++PropIt)
			{
				UProperty* Property = *PropIt;
				if (Property->GetClass()->ClassCastFlags & CASTCLASS_UIntProperty)
				{
					UIntProperty* intProp = Cast<UIntProperty>(Property);
					FString name = intProp->GetName();
					int val = intProp->GetPropertyValue_InContainer(target);
					node[TCHAR_TO_UTF8(*name)] = Json::Value(val);
				}
			}

			if (gameInstance->IsSaveStateValid(LocalUserNum))
			{
				gameInstance->ReadGlobalSaveState(LocalUserNum)->Save(LocalUserNum);
			}
		}
	}
}

void USettingsBlueprintFunctionLibrary::LoadGame(UObject* target, APlayerControllerBase* PlayerController)
{
	if (PlayerController)
	{
		auto gameInstance = PlayerController->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
		int LocalUserNum = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer())->GetSystemUserId();
		if (auto* save = gameInstance->EditGlobalSaveState(LocalUserNum))
		{
			if (gameInstance->IsSaveStateValid(LocalUserNum))
			{
				{
					Json::Value& node = save->GetSettingsNode();

					for (TFieldIterator<UProperty> PropIt(target->GetClass()); PropIt; ++PropIt)
					{
						UProperty* Property = *PropIt;
						if (Property->GetClass()->ClassCastFlags & CASTCLASS_UIntProperty)
						{
							UIntProperty* intProp = Cast<UIntProperty>(Property);
							FString name = intProp->GetName();
							//do validation on node to make sure.
							auto nodeValue = node[TCHAR_TO_UTF8(*name)];
							if (!nodeValue.isNull())
							{
								int val = nodeValue.asInt();
								intProp->SetPropertyValue_InContainer(target, val);
							}
						}
					}
				}
			}
		}
	}
}

bool USettingsBlueprintFunctionLibrary::GetTrackedStatByName(FString varName, const APlayerControllerBase * PlayerController)
{
	if (PlayerController)
	{
		auto gameInstance = PlayerController->GetWorld()->GetGameInstance<UDungeonsGameInstance>();

		auto localPlayer = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer());
		ensureMsgf(localPlayer, TEXT("Local player failed to initialize when trying to get Stat %s from save."), *varName);

		if (localPlayer) {
			int LocalUserNum = localPlayer->GetSystemUserId();
			if (auto* save = gameInstance->EditGlobalSaveState(LocalUserNum))
			{
				Json::Value& node = save->mTrackStatsNode;
				auto nodeValue = node[TCHAR_TO_UTF8(*varName)];
				if (!nodeValue.isNull())
				{
					
					return nodeValue.asBool();
				}
			}
		}
	}
	return false;
}

bool USettingsBlueprintFunctionLibrary::GetIsAccountLinked(const APlayerControllerBase * PlayerController)
{
	const TSharedPtr<const FUniqueNetId> UniqueNetId = PlayerController->GetUniqueNetId();
	if (UniqueNetId.IsValid() && online::getCrossplayOss()->IsDungeonsActive()) {
		auto xblNetId = online::getIdentityInterface()->GetUniqueNetIdFromType(*UniqueNetId, SubsystemType::Dungeons);
#if PLATFORM_SWITCH
		/* Playfab is not used for NSO + XBL linking on Nintendo Switch */
		return xblNetId.IsValid();
#else
		/* on PlayStation - also check if the account is linked via Playfab */
		return xblNetId.IsValid() && PlayfabServices::GetAccountLinkStatus(xblNetId->ToString()) == AccountLinkStatus::Linked;
#endif
	}
	return false;
}

bool USettingsBlueprintFunctionLibrary::GetCrossplay(const APlayerControllerBase * PlayerController)
{
	if (PlayerController)
	{
		auto gameInstance = PlayerController->GetWorld()->GetGameInstance<UDungeonsGameInstance>();

		auto localPlayer = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer());
		ensureMsgf(localPlayer, TEXT("Local player failed to initialize when trying to get %s from save."), "Crossplay");

		if (localPlayer) {
			int LocalUserNum = localPlayer->GetSystemUserId();
			if (auto* save = gameInstance->EditGlobalSaveState(LocalUserNum))
			{
				return save->mRecordedData.mCrossplay;
			}
		}
	}
	return false;
}

bool USettingsBlueprintFunctionLibrary::GetXblActive(const APlayerControllerBase* PlayerController)
{
	if (PlayerController)
	{
		auto gameInstance = PlayerController->GetWorld()->GetGameInstance<UDungeonsGameInstance>();

		auto localPlayer = Cast<UDungeonsLocalPlayer>(PlayerController->GetLocalPlayer());
		ensureMsgf(localPlayer, TEXT("Local player failed to initialize when trying to get %s from save."), "XblActive");

		if (localPlayer) {
			int LocalUserNum = localPlayer->GetSystemUserId();
			if (const auto* save = gameInstance->ReadGlobalSaveState(LocalUserNum))
			{
				return save->mRecordedData.mXblActive;
			}
		}
	}
	return false;
}

bool USettingsBlueprintFunctionLibrary::IsSignedInToXBL()
{
	return online::getCrossplayOss()->IsDungeonsActive() && online::getIdentityInterface()->GetLoginStatus(0) == ELoginStatus::LoggedIn;
}


bool USettingsBlueprintFunctionLibrary::HasSeenNews(const FTitleNews& news, const APlayerControllerBase* PlayerController){
	if (auto* globalSave = PlayerController->ReadGlobalSaveState()) {
		return globalSave->HasSeenNewsId(news.Id);
	}
	return false;
}

void USettingsBlueprintFunctionLibrary::SetSeenNews(const FTitleNews& news, APlayerControllerBase* PlayerController) {
	if (auto* globalSave = PlayerController->EditGlobalSaveState()) {
		if (!globalSave->HasSeenNewsId(news.Id)) {			
			globalSave->AddSeenNewsId(news.Id);
		}
	}
}

bool USettingsBlueprintFunctionLibrary::HasSeenMapNews(const FTitleNewsMapPopup& news, const APlayerControllerBase* PlayerController) {
	if (auto* globalSave = PlayerController->ReadGlobalSaveState()) {
		return globalSave->HasSeenNewsId(news.Id);
	}
	return false;
}

void USettingsBlueprintFunctionLibrary::SetSeenMapNews(const FTitleNewsMapPopup& news, APlayerControllerBase* PlayerController) {
	if (auto* globalSave = PlayerController->EditGlobalSaveState()) {
		if (!globalSave->HasSeenNewsId(news.Id)) {
			globalSave->AddSeenNewsId(news.Id);
		}
	}
}

bool USettingsBlueprintFunctionLibrary::HasSeenDLC6Cinematic(const APlayerControllerBase* PlayerController)
{
	if (auto* characterSerialiseComponent = PlayerController->GetCharacterSerializeComponent())	{
		UMediaSource* videoSource = LoadObject<UMediaSource>(NULL, TEXT("PlatformMediaSource'/Game/Movies/DLC6/End_C1_Stronghold_Intro_Platform.End_C1_Stronghold_Intro_Platform'"));
		return characterSerialiseComponent->HasVideoBeenPlayed(*videoSource);
	}
	return false;
}

void USettingsBlueprintFunctionLibrary::SetSeenDLC6Cinematic(ABasePlayerController* PlayerController)
{
	if (PlayerController)
	{
		if (auto* characterSerialiseComponent = PlayerController->GetCharacterSerializeComponent()) {
			UMediaSource* videoSource = LoadObject<UMediaSource>(NULL, TEXT("PlatformMediaSource'/Game/Movies/DLC6/End_C1_Stronghold_Intro_Platform.End_C1_Stronghold_Intro_Platform'"));
			if (!characterSerialiseComponent->HasVideoBeenPlayed(*videoSource)) {
				characterSerialiseComponent->SetVideoWasPlayed(*videoSource);
			}
		}
	}
}

ANTICHEAT_NO_OPTIMIZATION_BEGIN
bool USettingsBlueprintFunctionLibrary::IsDLCOwned(const UObject* WorldContextObject, EDLCName dlc) {
	ANTICHEAT_OBFUSCATE_BEGIN

		if (const auto* dlcDef = dlc::getChecked(dlc)) {	
#if !UE_BUILD_SHIPPING
			if (bUnlockAllDLC) {
				return true;
			}
#endif	
			return dlcDef->IsUnlockedByEntitlements(WorldContextObject->GetWorld()) && dlcDef->IsReleased();
		}

	return false;

	ANTICHEAT_OBFUSCATE_END
}
ANTICHEAT_NO_OPTIMIZATION_END

void USettingsBlueprintFunctionLibrary::UnlockAllDLC() {

	bUnlockAllDLC = true;
}