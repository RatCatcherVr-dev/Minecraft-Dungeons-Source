#include "Dungeons.h"
#include "CrossplaySessionResult.h"
#include "SessionSettings.h"
#include "Dungeons/online/crossplay/CrossplayOSS.h"

TArray<FSessionPlayerInfo> UDungeonsSessionAttributesEntity::GetSessionPlayerInfo(const FBlueprintSessionResult& Result) {
	const auto& SessionResult = Result.OnlineResult;
	TArray<FSessionPlayerInfo> XblPlayerInfo;
	const FOnlineSessionSetting* XblMemberData = SessionResult.Session.SessionSettings.Settings.Find(TEXT("XBLMEMBERS"));
	const FOnlineSessionSetting* PSNMemberData = SessionResult.Session.SessionSettings.Settings.Find(TEXT("PSNMEMBERS"));

	if (XblMemberData && SessionResult.Session.SessionInfo->GetSessionId().GetType().IsEqual(FName("DUNGEONS"))) {
		Json::Value JsonValue;
		Json::Reader reader;
		FString JsonString;

		XblMemberData->Data.GetValue(JsonString);
		if (!reader.parse(TCHAR_TO_ANSI(*JsonString), JsonValue)) {
			return XblPlayerInfo;
		}

		for (Json::Value::iterator itr = JsonValue.begin(); itr != JsonValue.end(); itr++) {
			FString GamerTag(itr.key().asString().c_str());
			FString Platform(ANSI_TO_TCHAR((*itr)["PLATFORM"].asCString()));
			XblPlayerInfo.Add(CreateXBLPlayer(GamerTag, Platform, SessionResult));
		}
	}
	else if (PSNMemberData && SessionResult.Session.SessionInfo->GetSessionId().GetType().IsEqual(PS4_SUBSYSTEM)) {
		//No XBL members. Check for PSN ones.

		if (PSNMemberData) {
			TArray<FString> psnMemberStrings;
			FString psnMembersString;
			PSNMemberData->Data.GetValue(psnMembersString);
			psnMembersString.ParseIntoArray(psnMemberStrings, TEXT(","), true);
			XblPlayerInfo.Add(CreatePSNHost(SessionResult));
			for (int i = 0; i < psnMemberStrings.Num(); i++) {
				XblPlayerInfo.Add(CreatePSNClient(psnMemberStrings[i], SessionResult));
			}
		}
	}
	else if (!online::getCrossplayOss()->IsNullActive()) {
		check(!"Session check failed!");
	}
	return XblPlayerInfo;
}

ESessionOnlineService UDungeonsSessionAttributesEntity::GetOnlineService(const FBlueprintSessionResult& BlueprintSession) {
	const auto SessionInfo = BlueprintSession.OnlineResult.Session.SessionInfo;
	if (SessionInfo->GetSessionId().GetType().IsEqual(PS4_SUBSYSTEM)) {
		return ESessionOnlineService::PSN;
	}
	if (SessionInfo->GetSessionId().GetType().IsEqual(FName(TEXT("DUNGEONS")))) {
		return ESessionOnlineService::XBL;
	}
	return ESessionOnlineService::UNKNOWN;

}

EPlayerPlatform ConvertFStringToPlayerPlatform(const FString& Platform) {
	if (!Platform.Compare("WINDOWS")) {
		return EPlayerPlatform::PC_PLATFORM;
	}
	if (!Platform.Compare("XBOXONE")) {
		return EPlayerPlatform::XBOXONE_PLATFORM;
	}
	if (!Platform.Compare("SWITCH")) {
		return EPlayerPlatform::SWITCH_PLATFORM;
	}
	if (!Platform.Compare("PS4")) {
		return EPlayerPlatform::PS4_PLATFORM;
	}
	return EPlayerPlatform::UNKNOWN;
}

ESessionOnlineService ConvertFStringToSessionService(const FString& Service) {
	if (Service.Compare("DUNGEONS")) {
		return ESessionOnlineService::XBL;
	}
	if (Service.Compare("PSN")) {
		return ESessionOnlineService::PSN;
	}
	return ESessionOnlineService::UNKNOWN;
}

FSessionPlayerInfo UDungeonsSessionAttributesEntity::CreateXBLPlayer(const FString& GamerTag, const FString& Platform, const FOnlineSessionSearchResult& SessionResult) {
	FSessionPlayerInfo PlayerInfo;
	SessionSettings ss(SessionResult);
	TArray<sessionSettings::FPlayerNames> playersNames = ss.GetPlayerNames();

	FString PSNName("");
	for (auto playerNames : playersNames) {
		if (GamerTag.Equals(playerNames.XBLGamerTag)) {
			PSNName = playerNames.PSNName;
			break;
		}
	}

	PlayerInfo.GamerTag = GamerTag;
	PlayerInfo.PSNName = PSNName;
	PlayerInfo.Platform = ConvertFStringToPlayerPlatform(Platform);
	PlayerInfo.OnlineService = ConvertFStringToSessionService(SessionResult.Session.SessionInfo->GetSessionId().GetType().ToString());
	PlayerInfo.ShouldShowCrossplayIcon = PLATFORM_PS4;
	PlayerInfo.ShouldShowPSNIcon = PLATFORM_PS4 && PlayerInfo.Platform == EPlayerPlatform::PS4_PLATFORM;
	PlayerInfo.isHost = GamerTag.Compare(SessionResult.Session.OwningUserName) == 0;

	return PlayerInfo;
}

void UDungeonsSessionAttributesEntity::SetupCommonPSNPlayerAttributes(FSessionPlayerInfo& PlayerInfo, const FOnlineSessionSearchResult& SessionResult) {
	PlayerInfo.GamerTag = "";
	PlayerInfo.Platform = EPlayerPlatform::PS4_PLATFORM;
	PlayerInfo.OnlineService = ConvertFStringToSessionService(SessionResult.Session.SessionInfo->GetSessionId().GetType().ToString());
	PlayerInfo.ShouldShowCrossplayIcon = false;
	PlayerInfo.ShouldShowPSNIcon = true;
}

FSessionPlayerInfo UDungeonsSessionAttributesEntity::CreatePSNHost(const FOnlineSessionSearchResult& SessionResult) {
	FSessionPlayerInfo PlayerInfo;
	PlayerInfo.PSNName = SessionResult.Session.OwningUserName;
	PlayerInfo.isHost = true;
	SetupCommonPSNPlayerAttributes(PlayerInfo, SessionResult);
	return PlayerInfo;
}

FSessionPlayerInfo UDungeonsSessionAttributesEntity::CreatePSNClient(const FString& PSNMember, const FOnlineSessionSearchResult& SessionResult) {
	FSessionPlayerInfo PlayerInfo;
	PlayerInfo.PSNName = PSNMember;
	PlayerInfo.isHost = false;
	SetupCommonPSNPlayerAttributes(PlayerInfo, SessionResult);
	return PlayerInfo;
}