#include "PCHOnlineDungeonsSubsystem.h"
#include "SessionProperties.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "utils.h"
#include "P2P/IpAddressDungeonsWebRTC.h"
#include "HrLog.h"

namespace sessionProperties {

static const FString VersionKey(_T("Version"));
static const FString OwningUserNameKey(_T("OwningUserName"));
static const FString OwningXUIDKey(_T("OwningXUID"));
static const FString Platform(OnlineSubsystemConstants::SETTING_PLATFORM.c_str());
static const FString Crossplay(OnlineSubsystemConstants::SETTING_CROSSPLAY.c_str());
static const FString IsPrivateGame("IsPrivateGame");
static const FString IsSessionReady("IsSessionReady");

static const FString MapName(OnlineSubsystemConstants::SETTING_MAP_NAME.c_str());
static const FString PlayerName(OnlineSubsystemConstants::SETTING_PLAYER_NAME.c_str());
static const FString RandomSeed(OnlineSubsystemConstants::SETTING_RANDOM_SEED.c_str());
static const FString GameVersion(OnlineSubsystemConstants::SETTING_GAME_VERSION.c_str());
static const FString MissionDifficulty(OnlineSubsystemConstants::SETTING_MISSION_DIFFICULTY.c_str());
static const FString LevelName(OnlineSubsystemConstants::SETTING_LEVEL_NAME.c_str());
static const FString LevelFilename(OnlineSubsystemConstants::SETTING_LEVEL_FILENAME.c_str());
static const FString ThreatLevel(OnlineSubsystemConstants::SETTING_THREAT_LEVEL.c_str());
static const FString ReconnectableGUIDS(OnlineSubsystemConstants::SETTINGS_RECONNECTABLE_GUIDS.c_str());
static const FString PlayerNames(OnlineSubsystemConstants::SETTING_PLAYERS_NAMES.c_str());

TOptional<utility::string_t> GetPropertyAsStringArray(const web::json::value& jsonValue, const utility::string_t& key) {
	if (!jsonValue.has_field(key) || !jsonValue.at(key).is_array()) {
		UE_LOG_ONLINE(Warning, TEXT("[SessionProperties] Could not find xbl property as string '%s'"), *xsapi::toFString(key));
		return {};
	}
	return jsonValue.at(key).serialize();
}

TOptional<utility::string_t> GetPropertyAsString(const web::json::value& jsonValue, const utility::string_t& key) {
	if (!jsonValue.has_field(key) || !jsonValue.at(key).is_string()) {
		UE_LOG_ONLINE(Warning, TEXT("[SessionProperties] Could not find xbl property as string '%s'"), *xsapi::toFString(key));
		return {};
	}
	return jsonValue.at(key).as_string();
}

TOptional<utility::string_t> GetPropertySerialize(const web::json::value& jsonValue, const utility::string_t& key) {
	if (!jsonValue.has_field(key)) {
		UE_LOG_ONLINE(Warning, TEXT("[SessionProperties] Could not find xbl property to string '%s'"), *xsapi::toFString(key));
		return {};
	}
	return jsonValue.at(key).serialize();
}

TOptional<int32> GetPropertyAsInt32(const web::json::value& jsonValue, const utility::string_t& key) {
	if (!jsonValue.has_field(key) || !jsonValue.at(key).is_number()) {
		UE_LOG_ONLINE(Warning, TEXT("[SessionProperties] Could not find xbl property as int32 '%s'"), *xsapi::toFString(key));
		return {};
	}
	return jsonValue.at(key).as_number().to_int32();
}

TOptional<uint64> GetPropertyAsUInt64(const web::json::value& jsonValue, const utility::string_t& key) {
	if (!jsonValue.has_field(key) || !jsonValue.at(key).is_number()) {
		UE_LOG_ONLINE(Warning, TEXT("[SessionProperties] Could not find xbl property as uint64 '%s'"), *xsapi::toFString(key));
		return {};
	}
	return jsonValue.at(key).as_number().to_uint64();
}

TOptional<uint32> GetPropertyAsUInt32(const web::json::value& jsonValue, const utility::string_t& key) {
	if (!jsonValue.has_field(key) || !jsonValue.at(key).is_number()) {
		UE_LOG_ONLINE(Warning, TEXT("[SessionProperties] Could not find xbl property as uint32 '%s'"), *xsapi::toFString(key));
		return {};
	}
	return jsonValue.at(key).as_number().to_uint32();
}

TOptional<utility::string_t> GetOwningXUID(const web::json::value& jsonValue) {
	const utility::string_t& key = xsapi::toString_t(OwningXUIDKey);
	if (const TOptional<utility::string_t> asString = GetPropertyAsString(jsonValue, key)) {
		return asString;
	}
	if (const TOptional<uint64> asUInt64 = GetPropertyAsUInt64(jsonValue, key)) {
		return xsapi::toString_t(asUInt64.GetValue());
	}
	return {};
}

void SetProperty(const FString& key, const FString& valueJson) {
	HrLogOnlyError(XblMultiplayerManagerLobbySessionSetProperties(TCHAR_TO_ANSI(*key), TCHAR_TO_ANSI(*valueJson), nullptr),
		"[SessionProperties] Setting xbl session property for key '" + key + "' with value '" + valueJson + "'" );
}

void SetPropertyAsString(const FString& name, const FString& value) {
	return SetProperty(name, "\"" + value + "\"");
}

void SetPropertyAsInt32(const FString& name, int32 value) {
	return SetProperty(name, FString::FromInt(value));
}

void SetPropertyAsUInt32(const FString& name, uint32 value)
{
	return SetProperty(name, std::to_string(value).data());
}

void SetPropertyAsUInt64(const FString& name, uint64 value){
	return SetProperty(name, std::to_string(value).data());
}

TOptional<SessionProperties> GetProperties(XblMultiplayerSessionHandle sessionHandle) {
	const auto xblProperties = XblMultiplayerSessionSessionProperties(sessionHandle);
	if (xblProperties == nullptr) {
		UE_LOG_ONLINE(Error, TEXT("[SessionProperties] Could get xbl properties from session handle."));
		return {};
	}
	const auto jsonValue = xbox::services::Utils::ParseJson(xblProperties->SessionCustomPropertiesJson);
	if (jsonValue.is_null()) {
		UE_LOG_ONLINE(Error, TEXT("[SessionProperties] Could parse json from xbl properties."));
		return {};
	}
	SessionProperties sp;
	sp.Version = GetPropertyAsInt32(jsonValue, xsapi::toString_t(VersionKey)).Get(-1);
	sp.OwningUserName = GetPropertyAsString(jsonValue, xsapi::toString_t(OwningUserNameKey)).Get(_T(""));
	sp.OwningXUID = GetOwningXUID(jsonValue).Get(_T(""));
	sp.Platform = GetPropertyAsString(jsonValue, xsapi::toString_t(Platform)).Get(_T("UNKNOWN"));
	sp.CrossplayEnabled = GetPropertyAsInt32(jsonValue, xsapi::toString_t(Crossplay)).Get(0) != 0;
	sp.IsPrivateGame = GetPropertyAsInt32(jsonValue, xsapi::toString_t(IsPrivateGame)).Get(0) != 0;
	sp.IsSessionReadyToJoin = GetPropertyAsInt32(jsonValue, xsapi::toString_t(IsSessionReady)).Get(0) != 0;

	sp.MapName = GetPropertyAsString(jsonValue, xsapi::toString_t(MapName)).Get(_T(""));
	sp.LevelName = GetPropertyAsString(jsonValue, xsapi::toString_t(LevelName)).Get(_T(""));
	sp.LevelFilename = GetPropertyAsString(jsonValue, xsapi::toString_t(LevelFilename)).Get(_T(""));
	sp.PlayerName = GetPropertyAsString(jsonValue, xsapi::toString_t(PlayerName)).Get(_T(""));
	sp.MissionDifficulty = GetPropertyAsString(jsonValue, xsapi::toString_t(MissionDifficulty)).Get(_T(""));
	sp.ThreatLevel = GetPropertyAsString(jsonValue, xsapi::toString_t(ThreatLevel)).Get(_T(""));
	sp.GameVersion = GetPropertyAsString(jsonValue, xsapi::toString_t(GameVersion)).Get(_T(""));
	sp.ReconnectableGUIDS = GetPropertyAsString(jsonValue, xsapi::toString_t(ReconnectableGUIDS)).Get(_T(""));
	sp.PlayerNames = GetPropertyAsStringArray(jsonValue, xsapi::toString_t(PlayerNames)).Get(_T(""));
	sp.RandomSeed = GetPropertyAsUInt32(jsonValue, xsapi::toString_t(RandomSeed)).Get(0);
	return sp;
}
	
void SetProperties(const FNamedOnlineSession* session) {
	SetPropertyAsInt32(VersionKey, SessionProperties::CURRENT_VERSION);
	SetPropertyAsString(OwningUserNameKey, session->OwningUserName);
	SetPropertyAsString(OwningXUIDKey, session->OwningUserId->ToString());
	SetPropertyAsString(Platform, ANSI_TO_TCHAR(environmentConfig::ConvertPlatformToString(environmentConfig::GetPlatform()).data()));
	SetPropertyAsInt32(IsPrivateGame, session->SessionSettings.bShouldAdvertise ? 0 : 1);
	for (const auto& setting : session->SessionSettings.Settings) {
		if (setting.Value.AdvertisementType != EOnlineDataAdvertisementType::DontAdvertise) {

			if (setting.Key.IsEqual(*PlayerNames))
			{
				SetProperty(setting.Key.ToString(), setting.Value.Data.ToString());
				continue;
			}
			switch (setting.Value.Data.GetType())
			{
			case EOnlineKeyValuePairDataType::Int32:
			{
				int32 Value;
				setting.Value.Data.GetValue(Value);
				SetPropertyAsInt32(setting.Key.ToString(), Value);
				break;
			}
			case EOnlineKeyValuePairDataType::String:
			{
				SetPropertyAsString(setting.Key.ToString(), setting.Value.Data.ToString());
				break;
			}
			case EOnlineKeyValuePairDataType::UInt32:
			{
				uint32 Value;
				setting.Value.Data.GetValue(Value);
				SetPropertyAsUInt32(setting.Key.ToString(), Value);
				break;
			}
			case EOnlineKeyValuePairDataType::UInt64:
			{
				uint64 Value;
				setting.Value.Data.GetValue(Value);
				SetPropertyAsUInt64(setting.Key.ToString(), Value);
				break;
			}
			default:
			{
				check(false && "Unknown type");
				break;
			}
			}
		}
	}
}
	
void SetXblMembers(XblMultiplayerSessionHandle SessionHandle, FOnlineSession& session)
{
	/* Get client(s) info */
	const XblMultiplayerSessionMember* Members = nullptr;
	size_t MemberCount = 0;
	if (HrFailed(XblMultiplayerSessionMembers(SessionHandle, &Members, &MemberCount), "[Session] XblMultiplayerSessionMembers"))
	{
		return;
	}

	std::vector<XBLMemberInfo> MemberInfo;
	const string_t DefaultPlatform = xsapi::toString_t(environmentConfig::ConvertPlatformToString(environmentConfig::PlatformType::UNKNOWN));

	for (size_t i = 0; i < MemberCount; i++)
	{
		const auto& Member = Members[i];
		auto Gamertag = FString(ANSI_TO_TCHAR(Member.Gamertag));
		const auto UserCustomProperties = xbox::services::Utils::ParseJson(Member.CustomPropertiesJson);

		MemberInfo.push_back({ 
			xsapi::toString_t(Gamertag),
			GetPropertyAsString(UserCustomProperties, OnlineSubsystemConstants::SETTING_PLATFORM).Get(DefaultPlatform)
			});
	}

	auto MemberInfoJson = ConvertMemberInfoToJson(MemberInfo);

	if (!MemberInfoJson.empty()) //Index is zero if no members added
	{
		FOnlineSessionSetting XblMemberSettings;
		XblMemberSettings.Data.SetValue(FString(MemberInfoJson.c_str()));
		XblMemberSettings.AdvertisementType = EOnlineDataAdvertisementType::DontAdvertise;
		session.SessionSettings.Set(FName(OnlineSubsystemConstants::SETTING_XBL_MEMBERS.c_str()), XblMemberSettings);
	}
}

string_t ConvertMemberInfoToJson(const std::vector<XBLMemberInfo>& MemberInfo)
{
	web::json::value JsonSettings;

	for (const auto& Member : MemberInfo)
	{
		JsonSettings[Member.GamerTag.c_str()][OnlineSubsystemConstants::SETTING_PLATFORM.c_str()] = web::json::value(Member.Platform);
	}

	return JsonSettings.serialize();
}

void SetIsReady(bool IsReady) {
	SetPropertyAsInt32(IsSessionReady, IsReady ? 1 : 0);
}

FOnlineSessionSearchResult BuildSessionFromXBLSession(
	const SessionProperties& properties,
	XblMultiplayerSessionHandle SessionHandle,
	const FString& HandleId,
	int32 PublicConnections, 
	int32 PublicOpenConnections) {

	UE_LOG_ONLINE(Log, TEXT("[SessionProperties] Building session."));
	FOnlineSessionSearchResult NewResult;
	FOnlineSession& session = NewResult.Session;
	session.SessionSettings.bAllowInvites = true;
	session.SessionSettings.bAllowJoinInProgress = true;
	session.SessionSettings.bAllowJoinViaPresenceFriendsOnly = true;
	session.SessionSettings.bAntiCheatProtected = true;
	session.SessionSettings.bIsDedicated = false;
	session.SessionSettings.bIsLANMatch = false;
	session.SessionSettings.bShouldAdvertise = !properties.IsPrivateGame;
	session.SessionSettings.bUsesPresence = true;
	session.SessionSettings.bUsesStats = true;
	
	session.NumOpenPublicConnections = PublicOpenConnections;
	session.SessionSettings.NumPublicConnections = PublicConnections;
	session.OwningUserName = properties.OwningUserName.c_str();
	if (!properties.OwningXUID.empty())
	{
		session.OwningUserId = MakeShareable(new FUniqueNetIdDungeons(std::stoull(properties.OwningXUID)));
	}

	/* Custom properties */
	FOnlineSessionSetting CustomDataSetting;
	CustomDataSetting.AdvertisementType = EOnlineDataAdvertisementType::ViaOnlineServiceAndPing;

	SetXblMembers(SessionHandle, session);

	CustomDataSetting.Data.SetValue(properties.CURRENT_VERSION);
	session.SessionSettings.Set(FName(*VersionKey), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.OwningUserName.c_str()));
	session.SessionSettings.Set(FName(*OwningUserNameKey), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.OwningXUID.c_str()));
	session.SessionSettings.Set(FName(*OwningXUIDKey), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.Platform.c_str()));
	session.SessionSettings.Set(FName(*Platform), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.MapName.c_str()));
	session.SessionSettings.Set(FName(*MapName), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.LevelName.c_str()));
	session.SessionSettings.Set(FName(*LevelName), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.LevelFilename.c_str()));
	session.SessionSettings.Set(FName(*LevelFilename), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.PlayerName.c_str()));
	session.SessionSettings.Set(FName(*PlayerName), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.MissionDifficulty.c_str()));
	session.SessionSettings.Set(FName(*MissionDifficulty), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.ThreatLevel.c_str()));
	session.SessionSettings.Set(FName(*ThreatLevel), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.GameVersion.c_str()));
	session.SessionSettings.Set(FName(*GameVersion), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.ReconnectableGUIDS.c_str()));
	session.SessionSettings.Set(FName(*ReconnectableGUIDS), CustomDataSetting);

	CustomDataSetting.Data.SetValue(FString(properties.PlayerNames.c_str()));
	session.SessionSettings.Set(FName(*PlayerNames), CustomDataSetting);

	CustomDataSetting.Data.SetValue(properties.RandomSeed);
	session.SessionSettings.Set(FName(*RandomSeed), CustomDataSetting);

	/* ------ */
	const auto correlationId = xsapi::toString_t(std::string(XblMultiplayerSessionGetInfo(SessionHandle)->CorrelationId));
	const TSharedPtr<FOnlineSessionInfoDungeons> NewSessionInfo = MakeShareable(new FOnlineSessionInfoDungeons(correlationId));
	NewSessionInfo->HostAddr = session.OwningUserId.IsValid() ? MakeShareable(new FInternetAddrDungeonsWebRTC(session.OwningUserId->ToString(), 0)) : nullptr;
	NewSessionInfo->HandleId = HandleId;

	
	session.SessionInfo = NewSessionInfo;
	UE_LOG_ONLINE(Log, TEXT("[SessionProperties] Built session: Version='%s', SessionId='%s', OwningUserId='%s', OwningUserName='%s', SessionInfo='%s', HostAddr='%s', HandleId='%s'")
		, *FString::FromInt(properties.Version)
		, *session.GetSessionIdStr()
		, session.OwningUserId.IsValid() ? *session.OwningUserId->ToString() : TEXT("")
		, *session.OwningUserName
		, *session.SessionInfo->ToString()
		, NewSessionInfo->HostAddr.IsValid() ? *NewSessionInfo->HostAddr->ToString(false) : TEXT("")
		, *NewSessionInfo->HandleId);
	return NewResult;
}

}
