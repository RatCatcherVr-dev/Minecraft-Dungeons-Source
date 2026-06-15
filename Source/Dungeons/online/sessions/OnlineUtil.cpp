#include "Dungeons.h"
#include "OnlineUtil.h"

#include "SessionSettings.h"
#include "DungeonsGameInstance.h"
#include "DungeonsUserManagement.h"
#include "game/actor/character/player/DungeonsLocalPlayer.h"
#include "online/reconnect/ReconnectUtil.h"
#include "platform/DungeonsVersion.h"
#include "DungeonsUserManagement.h"
#include "Dungeons/online/crossplay/CrossplayOSS.h"
#include "gamemodes/DungeonsGameStateBase.h"
#include <json/json.h>

#include "online/seasons/LiveOps.h"
#include <map>
#include <memory>

namespace online
{
const FString NullOnlineServiceName = "Null";
const FString DungeonsOnlineServiceName = "Dungeons";

TSharedPtr<Crossplay::OSS> crossplayOSS = {};

Crossplay::OSS* getCrossplayOss() {
	check(IsInGameThread());
	if (!crossplayOSS.IsValid())
	{
		crossplayOSS = MakeShared<Crossplay::OSS>();
	}
	return crossplayOSS.Get();
}

TSharedPtr<Crossplay::Session> getSessionInterface() {
	auto sessionInterface = getCrossplayOss()->GetSessionIF();
	if (!sessionInterface.IsValid()) {
		check(!"We depend on this.");
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid online session interface"));
		return nullptr;
	}
	return sessionInterface;
}

TSharedPtr<Crossplay::Identity> getIdentityInterface() {
	auto identityInterface = getCrossplayOss()->GetIdentityIF();
	if (!identityInterface.IsValid()) {
		check(!"We depend on this.");
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid identity interface"));
		return nullptr;
	}
	return identityInterface;
}

 TSharedPtr<Crossplay::Friends> getFriendsInterface() {
	const auto* onlineSystem = getCrossplayOss();

	auto friendInterface = onlineSystem->GetFriendsIF();
	if (!friendInterface.IsValid()) {
		check(!"We depend on this.");
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid friends interface"));
		return nullptr;
	}
	return friendInterface;
}

 TSharedPtr<Crossplay::ExternalUI> getExternalUIInterface() {
	 const auto* onlineSystem = getCrossplayOss();

	 auto externalUI = onlineSystem->GetExternalUIIF();
	 if (!externalUI.IsValid()) {
		 check(!"We depend on this.");
		 UE_LOG(LogOnline, Error, TEXT("Could not find a valid external UI interface"));
		 return nullptr;
	 }
	 return externalUI;
 }

 //We need this for PIE
std::map<UGameInstance*, std::shared_ptr<liveops::LiveOps>> liveOps = {};

liveops::LiveOps* getLiveOps(UGameInstance* GameInstance) {
	check(IsInGameThread() && GameInstance);
	auto iterator = liveOps.find(GameInstance);

	if (iterator == liveOps.end()) {
		iterator = liveOps.emplace(GameInstance, std::make_shared<liveops::LiveOps>(GameInstance)).first;
	}

	return iterator->second.get();
}

void createLiveOps(UGameInstance* GameInstance) {
	check(IsInGameThread() && GameInstance);

	if (liveOps.find(GameInstance) == liveOps.end()) {
		liveOps.emplace(GameInstance, std::make_shared<liveops::LiveOps>(GameInstance));
	}
}

void removeLiveOps(UGameInstance* GameInstance, int32 localUserNum) {
	if (auto DungeonsGameInstance = Cast<UDungeonsGameInstance>(GameInstance)) {
		if (DungeonsGameInstance->GetUserManager()->GetInitialUser() == localUserNum) {
			removeLiveOps(DungeonsGameInstance);
		}
	}
}

void removeLiveOps(UDungeonsGameInstance* GameInstance) {
	auto instance = liveOps.find(GameInstance);
	if (instance != liveOps.end()) {
		liveOps.erase(instance);
	}
	if (auto MinecraftAPI = GameInstance->GetMinecraftAPI()) {
		MinecraftAPI->Logout();
	}
}

liveops::LeaderboardsHandler* getLeaderboardsInterface(UGameInstance* GameInstance) {
	auto* leaderboards = getLiveOps(GameInstance)->GetLeaderboards();
	if (!leaderboards) {
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid Leaderboard"));
		return nullptr;
	}
	return leaderboards;
}

liveops::ChallengesHandler* getChallengesHandlerInterface(UGameInstance* GameInstance) {
	auto* challenges = getLiveOps(GameInstance)->GetChallenges();
	if (!challenges) {
		check(!"We depend on this.");
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid Objectives"));
		return nullptr;
	}
	return challenges;
}

liveops::SeasonsHandler* getSeasonsHandlerInterface(UGameInstance* GameInstance) {
	auto* seasons = getLiveOps(GameInstance)->GetSeasonHandler();
	if (!seasons) {
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid SeasonsHandler"));
		return nullptr;
	}
	return seasons;
}

liveops::ProgressHandler* getProgressHandlerInterface(UGameInstance* GameInstance) {
	auto* progressHandler = getLiveOps(GameInstance)->GetProgressHandler();
	if (!progressHandler) {
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid ProgressHandler"));
		return nullptr;
	}
	return progressHandler;
}

liveops::AdventurePointsHandler* getAdventurePointsHandlerInterface(UGameInstance* GameInstance) {
	auto* adventurePointsHandler = getLiveOps(GameInstance)->GetAdventurePointsHandler();
	if (!adventurePointsHandler) {
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid AdventurePointsHandler"));
		return nullptr;
	}
	return adventurePointsHandler;
}

liveops::RewardsHandler* getRewardsHandlerInterface(UGameInstance* GameInstance) {
	auto* rewardsHandler = getLiveOps(GameInstance)->GetRewardsHandler();
	if (!rewardsHandler) {
		UE_LOG(LogOnline, Error, TEXT("Could not find a valid RewardsHandler"));
		return nullptr;
	}
	return rewardsHandler;
}

std::string getSelectedSeason(UGameInstance* gameInstance) {
	if (auto event = getSeasonsHandlerInterface(gameInstance)->GetCurrentEvent()) {
		return event->GetName();
	}

	auto seasons = getSeasonsHandlerInterface(gameInstance)->GetSeasons();
	if (!seasons.empty()) {
		return seasons.front().GetName();
	}
	return "";
}

FNamedOnlineSession* getCurrentSession() {
	const auto crossplayOss = getCrossplayOss();
	if (!crossplayOss) {
		check(!"We depend on this.");
		return nullptr;
	}
	return crossplayOss->GetSessionIF()->GetNamedSession(DungeonsGameSessionName);
}

TOptional<FString> getCurrentSessionId() {
	const auto* session = getCurrentSession();
	if (session == nullptr) {
		UE_LOG(LogOnline, Log, TEXT("The dungeons game session was not found by name."));
		return {};
	}
	return session->GetSessionIdStr();
}

FString getLocalUserName(const UWorld* world) {
	Crossplay::OSS* const OnlineSub = getCrossplayOss();
	if (!OnlineSub) {
		return UKismetSystemLibrary::GetPlatformUserName();
	}

	const auto identity = OnlineSub->GetIdentityIF();
	if (!identity.IsValid()) {
		UE_LOG(LogOnline, Log, TEXT("(OSS) Identity interface is not available"));
		return UKismetSystemLibrary::GetPlatformUserName();
	}

	if (usingNullSubsystem(world)) {
		return UKismetSystemLibrary::GetPlatformUserName();
	}
	return identity->GetPlayerNickname(0);
}

void updateOnlineSession(const UWorld* world, bool forceUpdate) {
	const auto sessionInterface = getSessionInterface();
	if (!sessionInterface) {
		return;
	}

	auto* session = getCurrentSession();
	if (session == nullptr) {
		//no logging since this happens all the time.
		//UE_LOG(LogOnline, Log, TEXT("(OSS) Failed to update session data - no session"));
		return;
	}

	const auto& config = Cast<UDungeonsGameInstance>(world->GetGameInstance())->Configuration;
	const auto& levelSettings = config.GetLevelSettings();
	if(!levelSettings.IsSet()){
		UE_LOG(LogOnline, Log, TEXT("(OSS) Failed to update session data - no levelsettings in configuration"));
		return;
	}
	
	SessionSettings ss(session->SessionSettings);
	if (ss.Update(levelSettings.GetValue(), reconnect::getReconnectableGuids(world)) || forceUpdate) {
		session->SessionSettings = ss.Get();
		sessionInterface->UpdateSession(DungeonsGameSessionName, session->SessionSettings);
	}
}

bool usingNullSubsystem(const UWorld* world) {
	return matchOnlineServiceName(world, NullOnlineServiceName);
}

bool usingDungeonsSubsystem(const UWorld* world) {
	return matchOnlineServiceName(world, DungeonsOnlineServiceName);
}

bool matchOnlineServiceName(const UWorld* world, const FString& service) {
	FText ServiceName = getDefaultOnlineServiceName();
	return ServiceName.ToString().Equals(service, ESearchCase::IgnoreCase);
}

FText getDefaultOnlineServiceName() {
	return getCrossplayOss()->GetOnlineServiceName();
}

bool isLoggedInOnline(const UWorld* world) {
	if (auto identity = getIdentityInterface()) {

		int32 controllerId = world->GetGameInstance()->GetLocalPlayers()[0]->GetControllerId();

		if (identity->GetLoginStatus(controllerId) != ELoginStatus::LoggedIn) {
			UE_LOG(LogOnline, Log, TEXT("Local player is not logged in"));
			return false;
		}
		else {
			return true;
		}
	}
	return false;
}

TOptional<GameVersion> getVersion(const FString& versionString)
{
	FString majorVersion, minorVersion, res;

	if (!versionString.Split(".", &majorVersion, &res, ESearchCase::CaseSensitive))
	{
		return {};
	}

	if (!res.Split(".", &minorVersion, nullptr, ESearchCase::CaseSensitive))
	{
		return {};
	}

	if (!majorVersion.IsNumeric() || !minorVersion.IsNumeric())
	{
		return {};
	}

	return GameVersion(FCString::Atoi(*majorVersion), FCString::Atoi(*minorVersion));
}

bool isOnlineSession() {
	if (auto* session = getCurrentSession())
	{
		return !session->SessionSettings.bIsLANMatch;
	}
	return false;
}



bool isRunningMyGameVersion(const FOnlineSessionSearchResult& res) {
	SessionSettings ss(res);
	const FString& serverGameVersion = ss.GetGameVersion();
	const FString& myGameVersion = FNetworkVersion::GetProjectVersion();

	auto isLocalBuild = [](const FString& version) -> bool {
		return version == "LOCALBUILD";
	};

	if (isLocalBuild(GAME_VERSION) && isLocalBuild(serverGameVersion)) {
		UE_LOG(LogMultiplayer, Warning, TEXT("Showing server '%s' because the build versions are LOCALBUILD. This should not happen in production!"), *(res.Session.OwningUserName));
		return true;
	}

	if (isLocalBuild(serverGameVersion) != isLocalBuild(GAME_VERSION)) {
		return false;
	}

	TOptional<GameVersion> serverVersion = getVersion(serverGameVersion);

	if (!serverVersion.IsSet())
	{
		UE_LOG(LogMultiplayer, Log, TEXT("Could not parse server version number: %s"), *serverGameVersion);
		return false;
	}

	TOptional<GameVersion> myVersion = getVersion(myGameVersion);

	if (!myVersion.IsSet())
	{
		UE_LOG(LogMultiplayer, Log, TEXT("Could not parse my version number: %s"), *myGameVersion);
		return false;
	}

	if (serverVersion != myVersion)
	{
		UE_LOG(LogMultiplayer, Log, TEXT("Server '%s' is running a different version."), *(res.Session.OwningUserName));
		return false;
	}

	return true;
}

bool shouldShowSession(const FOnlineSessionSearchResult& res, const UWorld* world) {
	if (!res.Session.OwningUserId.IsValid()) {
		UE_LOG(LogMultiplayer, Warning, TEXT("Sessions without a valid owning user id will not be shown"));
		return false;
	}

	SessionSettings ss(res);
	if (ss.GetLevelName() == ELevelNames::squidcoast) {
		UE_LOG(LogMultiplayer, Log, TEXT("Games in squid coast will not be shown"));
		return false;
	}

	if (res.GetSessionIdStr() == getCurrentSessionId().Get("")) {
		UE_LOG(LogMultiplayer, Log, TEXT("My own session will not be shown"));
		return false;
	}
	if (!getCrossplayOss()->IsDungeonsActive()) {
		return true;
	}
	return isRunningMyGameVersion(res);
}

bool shouldShowSecondaryName(FString secondaryName) {
	return !sessionSettings::IsSamePlatform(sessionSettings::PlatformType::PS4_PLATFORM) && !secondaryName.Equals("");
}

bool shouldShowInvite(const FOnlineSessionSearchResult& res, const UWorld* world) {
	if (!res.Session.OwningUserId.IsValid()) 
	{
		UE_LOG(LogMultiplayer, Warning, TEXT("Invites without a valid owning user id will not be shown"));
		return false;
	}

	if (res.GetSessionIdStr() == getCurrentSessionId().Get("")) {
		UE_LOG(LogMultiplayer, Log, TEXT("Invite is from my own session"));
		return false;
	}

	if (online::getCrossplayOss()->IsPS4Active() && !online::IsCrossplayEnabled())
	{
		//PS4
		return false;
	}

	return isRunningMyGameVersion(res);
}

//Assumption: If we are not able to get GameInstance from either world or viewport we return false by default.
bool IsCrossplayEnabled() {
	if (!online::getCrossplayOss()->IsPS4Active()) {
		//On PC, Xbox & Switch crossplay is always enabled.
		return true;
	}

	if (!online::getCrossplayOss()->IsDungeonsActive())	{
		return false;
	}

	auto GetWorldSafer = []() {
		if (GEngine->GetWorld())
			return GEngine->GetWorld();
		return GEngine->GameViewport ? GEngine->GameViewport->GetWorld() : nullptr;
	};

	auto* world = GetWorldSafer();

	if (!world) {
		UE_LOG(LogOnline, Warning, TEXT("Failed to get world when getting crossplay setting - Defaulting to false!"));
		return false;
	}

	const UDungeonsGameInstance* DGameInstance = world->GetGameInstance<UDungeonsGameInstance>();

	if (!DGameInstance)	{
		return false;
	}

	UDungeonsUserManager* UserManager = DGameInstance->GetUserManager();

	if (!UserManager) {
		return false;
	}

	auto* GlobalSaveData = DGameInstance->EditGlobalSaveState(UserManager->GetInitialUserSystemId());

	if (!GlobalSaveData) {
		return false;
	}

	return GlobalSaveData->mRecordedData.mCrossplay;
}

bool IsSessionPlatformCompliant(const FOnlineSessionSearchResult& res, bool IsCrossplay) {
	bool SamePlatform = false;
	bool CrossPlayEnabled = false;
	const auto PlatformSetting = res.Session.SessionSettings.Settings.Find(sessionSettings::SETTING_PLATFORM);
	if (PlatformSetting && PlatformSetting->Data.GetType() == EOnlineKeyValuePairDataType::Type::String)
	{
		FString OutData;
		PlatformSetting->Data.GetValue(OutData);
		SamePlatform = sessionSettings::GetPlatform() == sessionSettings::ConvertToPlatform(OutData);
	}

	const auto CrossplayEnabledSetting = res.Session.SessionSettings.Settings.Find(sessionSettings::SETTING_CROSSPLAY);
	if (CrossplayEnabledSetting && CrossplayEnabledSetting->Data.GetType() == EOnlineKeyValuePairDataType::Bool)
	{
		CrossplayEnabledSetting->Data.GetValue(CrossPlayEnabled);
	}

	if (SamePlatform || (CrossPlayEnabled && IsCrossplay))
	{
		return true;
	}
	return false;
}

bool SetXblActive(bool newValue, APlayerControllerBase* PlayerControllerBase)
{
	if (PlayerControllerBase)
	{
		auto gameInstance = PlayerControllerBase->GetWorld()->GetGameInstance<UDungeonsGameInstance>();
		int LocalUserNum = Cast<UDungeonsLocalPlayer>(PlayerControllerBase->GetLocalPlayer())->GetSystemUserId();
		if (auto* save = gameInstance->EditGlobalSaveState(LocalUserNum))
		{
			if (save->mRecordedData.mXblActive != newValue) {
				save->mRecordedData.mXblActive = newValue;
				save->Save(LocalUserNum);
			}
			return true;
		}
	}
	return false;
}

bool IsUsingOnlineFeatures()
{
#if UE_BUILD_SHIPPING
	return true;
#else
	bool inEditor = WITH_EDITOR;
	bool gameFlag = FParse::Param(FCommandLine::Get(), TEXT("game"));
	bool lanFlag = FParse::Param(FCommandLine::Get(), TEXT("lan"));
	bool onlineFlag = FParse::Param(FCommandLine::Get(), TEXT("online"));

	if (lanFlag) {
		return false;
	}
	if (onlineFlag) {
		return true;
	}
	return !inEditor || (inEditor && gameFlag);
#endif

}

bool IsUsingOnlineFeaturesInPIE()
{
#if UE_BUILD_SHIPPING
	return false;
#else
	bool inEditor = WITH_EDITOR;
	bool gameFlag = FParse::Param(FCommandLine::Get(), TEXT("game"));
	bool lanFlag = FParse::Param(FCommandLine::Get(), TEXT("lan"));
	bool onlineFlag = FParse::Param(FCommandLine::Get(), TEXT("online"));

	if (lanFlag) {
		return false;
	}
	if (gameFlag) {
		return false;
	}
	return inEditor && onlineFlag;
#endif
}

TOptional<FString> DynamicNamespace = {};

void SetDynamicNamespace(FString dynamicNamespace) {
	DynamicNamespace = dynamicNamespace;
}

FString GetMinecraftAPINamespace() {
#if UE_BUILD_SHIPPING
	return "default";
#endif
	if (DynamicNamespace) {
		return DynamicNamespace.GetValue();
	}
	FString val;
	if (FParse::Value(FCommandLine::Get(), TEXT("namespace="), val)) {
		return val;
	}
	return "default";
}

bool IsUsingCachedEntitlements() {
	auto platformType = sessionSettings::GetPlatform();
	return platformType == sessionSettings::PlatformType::PS4_PLATFORM ||
		platformType == sessionSettings::PlatformType::XBOXONE_PLATFORM ||
		platformType == sessionSettings::PlatformType::SWITCH_PLATFORM;
}

FString GameVersion::Get() const
{
	return FString(FString::FromInt(mMajor) + FString::FromInt(mMinor));
}
}

namespace sessionSettings {

	sessionSettings::PlatformType GetPlatform()
	{
#if PLATFORM_WINDOWS
		return PlatformType::WINDOWS_PLATFORM;
#elif PLATFORM_XBOXONE
		return PlatformType::XBOXONE_PLATFORM;
#elif PLATFORM_SWITCH
		return PlatformType::SWITCH_PLATFORM;
#elif PLATFORM_PS4
		return PlatformType::PS4_PLATFORM;
#else
		static_assert(false, "No platform specified");
#endif
	}

	std::string ConvertPlatformToString(PlatformType Platform)
	{
		switch (Platform)
		{
		case sessionSettings::PlatformType::WINDOWS_PLATFORM:
			return "WINDOWS";
		case sessionSettings::PlatformType::XBOXONE_PLATFORM:
			return "XBOXONE";
		case sessionSettings::PlatformType::SWITCH_PLATFORM:
			return "SWITCH";
		case sessionSettings::PlatformType::PS4_PLATFORM:
			return "PS4";
		default:
			return "UNKNOWN";
		}
	}

	sessionSettings::PlatformType ConvertToPlatform(const FString& Platform)
	{
		if (!Platform.Compare("WINDOWS"))
		{
			return PlatformType::WINDOWS_PLATFORM;
		}
		if (!Platform.Compare("XBOXONE"))
		{
			return PlatformType::XBOXONE_PLATFORM;
		}
		if (!Platform.Compare("SWITCH"))
		{
			return PlatformType::SWITCH_PLATFORM;
		}
		if (!Platform.Compare("PS4"))
		{
			return PlatformType::PS4_PLATFORM;
		}
		return PlatformType::UNKNOWN;
	}

	bool IsSamePlatform(PlatformType Platform) {
		return GetPlatform() == Platform;
	}

	bool CanPlatformRespondToOutOfTitleInvites(PlatformType Platform) {
		switch (Platform) {
		case sessionSettings::PlatformType::XBOXONE_PLATFORM:
			return true;
		case sessionSettings::PlatformType::WINDOWS_PLATFORM:
		case sessionSettings::PlatformType::SWITCH_PLATFORM:
		case sessionSettings::PlatformType::PS4_PLATFORM:
		case sessionSettings::PlatformType::UNKNOWN:
		default:
			return false;
		}
	}

	TArray<FPlayerNames> GetPlayerNames() {
		TArray<FPlayerNames> playerNames;
		if (auto* gameState = GEngine->GetWorldContexts()[0].World()->GetGameState<ADungeonsGameStateBase>()) {
			for (auto playerState : gameState->GetPlayerStates()) {
				FPlayerNames player;
				player.PSNName = "";
				player.XBLGamerTag = "";

				if (playerState->GetPlayerSecondaryDisplayName().Equals("")) {
					player.XBLGamerTag = playerState->GetPlayerDisplayName();
				}
				else if (GetPlatform() == PlatformType::PS4_PLATFORM) {
					player.PSNName = playerState->GetPlayerDisplayName();
					player.XBLGamerTag = playerState->GetPlayerSecondaryDisplayName();
				}
				else {
					player.XBLGamerTag = playerState->GetPlayerDisplayName();
					player.PSNName = playerState->GetPlayerPrimaryDisplayName();
				}
				playerNames.Add(player);
			}
		}
		return playerNames;
	}

	FString GetPlayerNamesAsString()
	{
		Json::Value root;
		const TArray<FPlayerNames> PlayerInfo = GetPlayerNames();
		for (const auto& player : PlayerInfo) {
			Json::Value playerNode;
			playerNode[SETTING_PSNNAME] = std::string(TCHAR_TO_UTF8(*player.PSNName));
			playerNode[SETTING_XBLGAMERTAG] = std::string(TCHAR_TO_UTF8(*player.XBLGamerTag));
			root.append(playerNode);
		}
		Json::FastWriter writer;
		return FString(writer.write(root).c_str());
	}
	FString GetInviteSenderName(const TArray<FPlayerNames>& PlayerNames, const FString& PlayerName) {
		auto InviteComparator = [&PlayerName](const FPlayerNames& Player) -> bool {
			return Player.XBLGamerTag == PlayerName;
		};

		if (online::getCrossplayOss()->IsPS4Active()) {
			const auto* FoundPlayer = PlayerNames.FindByPredicate(InviteComparator);
			if (!FoundPlayer) {
				return PlayerName;
			}
			else {
				return FoundPlayer->PSNName == "" ? FoundPlayer->XBLGamerTag : FoundPlayer->PSNName;
			}
		}
		else {
			return PlayerName;
		}
	}
}
FName UOnlineUtil::GetSessionType()
{
	auto* session = online::getSessionInterface()->GetNamedSession(DungeonsGameSessionName);
	return session ? session->SessionInfo->GetSessionId().GetType() : FName("");
}

SessionType UOnlineUtil::GetSessionEnumType() {
	if (auto* session = online::getSessionInterface()->GetNamedSession(DungeonsGameSessionName)) {
		if (session->SessionInfo->GetSessionId().GetType().IsEqual("DUNGEONS")) {
			return SessionType::CROSSPLAY;
		}
		else if (session->SessionInfo->GetSessionId().GetType().IsEqual("PS4")) {
			return SessionType::PS4;
		}
	}
	return SessionType::UNKNOWN;
}

bool UOnlineUtil::IsOnlineSession()
{
	return online::isOnlineSession();
}
