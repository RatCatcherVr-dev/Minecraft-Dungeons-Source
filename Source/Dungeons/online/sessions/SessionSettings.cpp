#include "Dungeons.h"
#include "SessionSettings.h"
#include "util/EnumUtil.h"
#include "platform/DungeonsVersion.h"
#include "game/levels.h"
#include "FindSessionsCallbackProxy.h"
#include "game/LevelSettings.h"
#include "OnlineUtil.h"
#include "util/StringUtil.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "gamemodes/DungeonsGameStateBase.h"
#include "online/crossplay/CrossplayOSS.h"
#include "game/mission/difficulty/MissionDifficulty.h"


namespace sessionSettings {

	static const FName SETTING_XBL_MEMBERS("XBLMEMBERS");

	static const char* SETTING_MAP_NAME("UNREAL_MAP_NAME");
	static const char* SETTING_PLAYER_NAME("PLAYER_NAME");
	static const char* SETTING_RANDOM_SEED("RANDOM_SEED");
	static const char* SETTING_GAME_VERSION("GAME_VERSION");
	static const char* SETTING_MISSION_DIFFICULTY("MISSION_DIFFICULTY");
	static const char* SETTING_LEVEL_NAME("LEVEL_NAME");
	static const char* SETTING_LEVEL_NAME_ENUM("LEVEL_NAME_ENUM");
	static const char* SETTING_LEVEL_FILENAME("LEVEL_FILENAME");
	static const char* SETTING_THREAT_LEVEL("THREAT_LEVEL");
	static const char* SETTING_ENDLESS_STRUGGLE("ENDLESS_STRUGGLE");
	static const char* SETTING_EMERGENT_DIFFICULTY_RAIDDIFFICULTY("EMERGENT_DIFFICULTY_RAIDDIFFICULTY");
	static const char* SETTING_EMERGENT_DIFFICULTY_AFFECTORSNUM("EMERGENT_DIFFICULTY_AFFECTORSNUM");
	static const char* SETTINGS_RECONNECTABLE_GUIDS("RECONNECTABLE_GUIDS");
	static const char* SETTING_PLAYERS_NAMES("PLAYERS_NAMES");

	bool equals(const FOnlineSessionSettings& lhs, const FOnlineSessionSettings& rhs) {
		if (lhs.Settings.Num() != rhs.Settings.Num()) {
			return false;
		}
		for (FSessionSettings::TConstIterator It(lhs.Settings); It; ++It) {
			const FOnlineSessionSetting& setting = It.Value();
			FString leftValue;
			if (!rhs.Get(It.Key(), leftValue)) {
				return false;
			}
			FString rightValue;
			setting.Data.GetValue(rightValue);
			if (leftValue != rightValue) {
				return false;
			}
		}
		return true;
	}

	FString arrayToString(const TArray<FString>& arr) {
		return FString::Join(arr, TEXT(","));
	}

	TArray<FString> stringToArray(const FString& commaSeparatedString) {
		TArray<FString> arr;
		commaSeparatedString.ParseIntoArray(arr, TEXT(","));
		return arr;
	}

	FString getLocalUserName(UWorld* world) {
		UDungeonsUserManager* userManagement = world->GetGameInstance<UDungeonsGameInstance>()->GetUserManager();
		int initialUser = userManagement->GetInitialUserSystemId();
		return userManagement->GetLocalUserName(initialUser);
	}

}

SessionSettings::SessionSettings() {
}

SessionSettings::SessionSettings(const FOnlineSessionSettings& settings) : Settings(settings) {

}

SessionSettings::SessionSettings(const FOnlineSessionSearchResult& osr) : SessionSettings(osr.Session.SessionSettings) {
}

SessionSettings::SessionSettings(const FBlueprintSessionResult& bsr) : SessionSettings(bsr.OnlineResult) {
}

SessionSettings::SessionSettings(const FString& localPlayerName, const FLevelSettings& levelSettings, bool isPrivate, bool isCrossplayEnabled) {
	Settings.bIsLANMatch = false;
	Settings.bAllowJoinInProgress = true;
	Settings.bAllowJoinViaPresenceFriendsOnly = false;
	Settings.bAllowInvites = true;
	Settings.bIsDedicated = false;
	SetPrivate(isPrivate);

	Settings.Set(SETTING_MATCHING_HOPPER, FString("Deathmatch"), EOnlineDataAdvertisementType::DontAdvertise);
	Settings.Set(SETTING_SESSION_TEMPLATE_NAME, DungeonsGameSessionName.ToString(), EOnlineDataAdvertisementType::DontAdvertise);

	Settings.Set(sessionSettings::SETTING_PLAYER_NAME, localPlayerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(
		sessionSettings::SETTING_GAME_VERSION, 
		FString(GAME_VERSION).Equals("LOCALBUILD") ? FString(GAME_VERSION) : FNetworkVersion::GetProjectVersion(),
		EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	Settings.Set(sessionSettings::SETTING_CROSSPLAY, isCrossplayEnabled ? 1 : 0, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	Update(levelSettings, {});
}

bool SessionSettings::Update(const FLevelSettings& levelSettings, const TArray<FString>& reconnectableGuids) {
	// @warn: If you update non Set methods - make sure to update equals.
	const auto oldSettings = Settings;
	
	Settings.Set(sessionSettings::SETTING_MAP_NAME, levelSettings.getUnrealMapName(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_LEVEL_NAME, GetEnumValueToStringStripped(levelSettings.getLevelName()), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_LEVEL_FILENAME, levelSettings.getLevelFilename(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_MISSION_DIFFICULTY, GetEnumValueToStringStripped(levelSettings.getDifficulty()), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_THREAT_LEVEL, GetEnumValueToStringStripped(levelSettings.getThreatLevel()), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_ENDLESS_STRUGGLE, static_cast<int32>(levelSettings.getEndlessStruggle().Value), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_EMERGENT_DIFFICULTY_RAIDDIFFICULTY, static_cast<int32>(levelSettings.getEmergentDifficulty().raidDifficulty), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_EMERGENT_DIFFICULTY_AFFECTORSNUM, static_cast<int32>(levelSettings.getEmergentDifficulty().midGameAffectorsNum), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_RANDOM_SEED, static_cast<int32>(levelSettings.getSeed()), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTINGS_RECONNECTABLE_GUIDS, sessionSettings::arrayToString(reconnectableGuids), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_LEVEL_NAME_ENUM, FString::FromInt(static_cast<int>(levelSettings.getLevelName())), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	Settings.Set(sessionSettings::SETTING_PLAYERS_NAMES, sessionSettings::GetPlayerNamesAsString(), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	return !sessionSettings::equals(Settings, oldSettings);
}

void SessionSettings::SetPrivate(bool isPrivate) {
	Settings.bShouldAdvertise = !isPrivate;
	Settings.bUsesPresence = !isPrivate;
	Settings.NumPublicConnections = isPrivate ? 0 : PlatformMaxPlayers;
	Settings.NumPrivateConnections = isPrivate ? PlatformMaxPlayers : 0;
	Settings.bAllowJoinViaPresence = !isPrivate;
}

void SessionSettings::SetLanMatch(bool isLan) {
	Settings.bIsLANMatch = isLan;
}

bool SessionSettings::IsPrivate() const {
	return !Settings.bShouldAdvertise;
}

FString SessionSettings::GetUnrealMapName() const {
	if (!online::getCrossplayOss()->IsDungeonsActive()) {
		FString mapName;
		if (Settings.Get(sessionSettings::SETTING_MAP_NAME, mapName)) {
			return mapName;
		}
	}
	if (auto mapName = Get(sessionSettings::SETTING_MAP_NAME)) {
		return mapName.GetValue();
	}
	UE_LOG(LogOnline, Error, TEXT("SessionSettings was missing SETTING_MAP_NAME."));
	return "invalid";
}

FText SessionSettings::GetLevelDisplayName() const {
	return missions::get(GetLevelName()).getNameText();
}

EGameDifficulty SessionSettings::GetDifficulty() const {
	auto difficultyStr = Get(sessionSettings::SETTING_MISSION_DIFFICULTY);
	if (!difficultyStr) {
		return EGameDifficulty::Difficulty_1;
	}
	if (auto difficulty = EnumValueFromString(EGameDifficulty, difficultyStr.GetValue())) {
		return difficulty.GetValue();
	}
	UE_LOG(LogOnline, Error, TEXT("Could not decode difficulty from json - falling back on difficulty 1."));
	return EGameDifficulty::Difficulty_1;
}

FString SessionSettings::GetLevelNameStr() const {
	return Get(sessionSettings::SETTING_LEVEL_NAME).Get("");
}

ELevelNames SessionSettings::GetLevelName() const {
	if (!online::getCrossplayOss()->IsDungeonsActive()) {
		FString enumIntAsString;
		if (Settings.Get(sessionSettings::SETTING_LEVEL_NAME_ENUM, enumIntAsString)) {
			return static_cast<ELevelNames>(FCString::Atoi(*enumIntAsString));
		}
	}
	return EnumValueFromString(ELevelNames, GetLevelNameStr()).Get(ELevelNames::Invalid);
}

FString SessionSettings::GetLevelFilename() const {
	return Get(sessionSettings::SETTING_LEVEL_FILENAME).Get("");
}

FString SessionSettings::GetThreatLevelStr() const {
	return Get(sessionSettings::SETTING_THREAT_LEVEL).Get("");
}

EThreatLevel SessionSettings::GetThreatLevel() const {
	return EnumValueFromString(EThreatLevel, GetThreatLevelStr()).Get(EThreatLevel::Invalid);
}

FString SessionSettings::GetEndlessStruggleStr() const { 
	return Get(sessionSettings::SETTING_ENDLESS_STRUGGLE).Get("");
}

FEndlessStruggle SessionSettings::GetEndlessStruggle() const {
	return {FCString::Atoi( *GetEndlessStruggleStr())};
}

FString SessionSettings::GetEmergentDifficultyRaidDifficultyStr() const
{
	return Get(sessionSettings::SETTING_EMERGENT_DIFFICULTY_RAIDDIFFICULTY).Get("");
}

FString SessionSettings::GetEmergentDifficultyAffectorsNumStr() const
{
	return Get(sessionSettings::SETTING_EMERGENT_DIFFICULTY_AFFECTORSNUM).Get("");
}

FEmergentDifficulty SessionSettings::GetEmergentDifficulty() const
{
	return { FCString::Atoi(*GetEmergentDifficultyRaidDifficultyStr()), FCString::Atoi(*GetEmergentDifficultyAffectorsNumStr()) };
}

FString SessionSettings::GetGameVersion() const {
	return Get(sessionSettings::SETTING_GAME_VERSION).Get("");
}

FString SessionSettings::GetPlayerName() const {
	if (!online::getCrossplayOss()->IsDungeonsActive()) {
		FString playername;
		if (Settings.Get(sessionSettings::SETTING_PLAYER_NAME, playername)) {
			return playername;
		}
	}
	return Get(sessionSettings::SETTING_PLAYER_NAME).Get("");
}

RandomSeed SessionSettings::GetRandomSeed() const {
	if (const auto seed = Get(sessionSettings::SETTING_RANDOM_SEED)) {
		return FCString::Atoi(*seed.GetValue());
	}
	UE_LOG(LogOnline, Error, TEXT("Could not decode RandomSeed from json - falling back to 4294967296."));
	return -1;
}

FLevelSettings SessionSettings::GetLevelSettings() const {
	return FLevelSettings(
		FMissionState(
			FMissionDifficulty{
				GetLevelName(),
				GetDifficulty(),
				GetThreatLevel(),
				GetEndlessStruggle(),
			},
			GetRandomSeed()
		),
		GetEmergentDifficulty(),
		GetUnrealMapName(),
		GetLevelFilename(),
		{}
	);
}

bool SessionSettings::CanReconnect(const FString& guid) const {
	if (auto guids = Get(sessionSettings::SETTINGS_RECONNECTABLE_GUIDS)) {
		return guids.GetValue().Contains(guid);
	}
	return false;
}

TArray<sessionSettings::FPlayerNames> SessionSettings::GetPlayerNames() const {
	TArray<sessionSettings::FPlayerNames> playerNames;
	FString PlayerNamesSetting = Get(sessionSettings::SETTING_PLAYERS_NAMES).Get("");
	std::string playerNamesString = std::string(TCHAR_TO_UTF8(*PlayerNamesSetting));
	Json::Value root;
	Json::Reader reader;

	if (reader.parse(playerNamesString, root))
	{
		for (Json::Value::iterator itr = root.begin(); itr != root.end(); itr++) {
			sessionSettings::FPlayerNames sessionMember({"", ""});
			Json::Value tmp = (*itr)[sessionSettings::SETTING_PSNNAME];
			if (!tmp.isNull()) {
				sessionMember.PSNName = ANSI_TO_TCHAR(tmp.asCString());
			}
			tmp = (*itr)[sessionSettings::SETTING_XBLGAMERTAG];
			if (!tmp.isNull()) {
				sessionMember.XBLGamerTag = ANSI_TO_TCHAR(tmp.asCString());
			}
			playerNames.Add(sessionMember);
		}
	}
	else {
		UE_LOG(LogOnline, Warning, TEXT("Failed to parse players names in session settings"));
	}

	return playerNames;
}

const FOnlineSessionSettings& SessionSettings::Get() const {
	return Settings;
}

FOnlineSessionSettings& SessionSettings::Get() {
	return Settings;
}

TOptional<FString> SessionSettings::Get(const FName& key) const {
	FString Value;
	if (Settings.Get(key, Value)) {
		return Value;
	}
	UE_LOG(LogOnline, Warning, TEXT("Missing key '%s' in session settings"), *key.ToString());
	return {};
}

SessionSettings online::createSessionSettings(UWorld* world, const FLevelSettings& LevelSettings, bool isPrivate, bool isCrossplayEnabled) {
	return SessionSettings(getLocalUserName(world), LevelSettings, isPrivate, isCrossplayEnabled);
}
