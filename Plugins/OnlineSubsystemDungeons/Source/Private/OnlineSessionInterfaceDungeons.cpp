#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineSessionInterfaceDungeons.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemTypes.h"
#include "SocketSubsystem.h"
#include "OnlineIdentityDungeons.h"
#include "xsapiServicesInclude.h"

#include "OnlineSubsystemUtils.h"
#include "P2P/Peer2PeerManager.h"

#include "Xal/xal_user.h"
#include "xal_types.h"

#include "types.h"

#if PLATFORM_SWITCH || PLATFORM_PS4
#include "utils.h"
#else
#include "AllowWindowsPlatformAtomics.h"
#include "AllowWindowsPlatformTypes.h"
#include "utils.h"
#include "HideWindowsPlatformTypes.h"
#include "HideWindowsPlatformAtomics.h"
#endif

#if PLATFORM_SWITCH
#include "Switch/SwitchSocketIncludes.h"
#elif PLATFORM_PS4

#else
#pragma warning(disable:4668)
#include "AllowWindowsPlatformAtomics.h"
#include "AllowWindowsPlatformTypes.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "HideWindowsPlatformTypes.h"
#include "HideWindowsPlatformAtomics.h"
#pragma warning(default:4668)
#endif

#include "ScopeLock.h"
#include "AsyncTasks/OnlineAsyncTaskDungeonsFindLobbies.h"
#include "AsyncTasks/OnlineAsyncTaskDungeonsTriggers.h"
#include "SharedPointer.h"
#include "Engine/EngineBaseTypes.h"
#include "SessionProperties.h"
#include "Dungeons/online/OnlineCommon.h"
#include "GameSession/OnlineHostedSession.h"
#include "GameSession/OnlineClientSession.h"
#include "HrLog.h"
#include "IPAddress.h"
#include "GameSession/OnlineGameSession.h"
#include "multiplayer.h"
#include <thread>
#include "GameSession/OnlineGameSessionBase.h"

#if !PLATFORM_XBOXONE && !(defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1)
FOnlineSessionDungeonsPtr dungeonsSession::platform::Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem)
{
	return MakeShared<FOnlineSessionDungeons, ESPMode::ThreadSafe>(InDungeonsSubsystem);
}
#endif
namespace dungeonsSession
{
	void DoWork(const std::function<void(const XblMultiplayerEvent&)>& HandleEvent)
	{
		const XblMultiplayerEvent * MultiplayerEvents = nullptr;
		size_t MultiplayerEventCount = 0;
		HrLogOnlyError(XblMultiplayerManagerDoWork(&MultiplayerEvents, &MultiplayerEventCount), "[Session] Multiplayer manager do work");

		for (size_t i = 0; i < MultiplayerEventCount; i++)
		{
			const auto& mpEvent = MultiplayerEvents[i];
			xsapi::LogEvent(mpEvent);
			HandleEvent(mpEvent);
		}
	}
}

uint32 FOnlineSessionDungeons::JoinSessionInternal(int32 PlayerNum, FNamedOnlineSession* Session, const FOnlineSession* SearchSession, const FName SessionName)
{
	check(Session != nullptr);

	uint32 Result = E_FAIL;

	if (Session->SessionInfo.IsValid() && SearchSession != nullptr && SearchSession->SessionInfo.IsValid())
	{
		//Copy session
		const FOnlineSessionInfoDungeons* SearchSessionInfo = static_cast<const FOnlineSessionInfoDungeons*>(SearchSession->SessionInfo.Get());
		FOnlineSessionInfoDungeons* SessionInfo = static_cast<FOnlineSessionInfoDungeons*>(Session->SessionInfo.Get());

		if (!SearchSessionInfo->HostAddr.IsValid() || SearchSessionInfo->HandleId.IsEmpty())
		{
			UE_LOG_ONLINE(Warning, TEXT("[Session] Unable to join session - HostAddr & HandleId have to be set"));
			return E_FAIL;
		}

		/* Setup connection info */
		SessionInfo->SessionId = SearchSessionInfo->SessionId;
		SessionInfo->HostAddr = SearchSessionInfo->HostAddr;
		SessionInfo->HandleId = SearchSessionInfo->HandleId;

		auto sessionId = Session->SessionInfo->GetSessionId().ToString();

		XblUserHandle userHandle = GetIdentityInterface()->GetXBLUserHandle(PlayerNum);

		GameSession = FOnlineClientSession::JoinSession(
			GetIdentityInterface(),
			userHandle,
			SessionInfo->HandleId,
			Subsystem->GetQueueHandle(),
			FCString::Strtoui64(*Session->OwningUserId->ToString(), nullptr, 10),
			[this](FOnlineClientSession::EJoinResult result) { HandleJoinSessionComplete(result); },
			[this] { HandleHostChanged(); },
			[this] { HandleClientDisconnectedFromService(); }
		);

		if (!GameSession) {
			UE_LOG_ONLINE(Log, TEXT("[Session] Joining lobby (Address='%s', HandleId='%s')"), *SessionInfo->HostAddr->ToString(true), *sessionId);
			return E_FAIL;
		}
		else {
			Result = ONLINE_SUCCESS;
		}
	}

	return Result;
}

void FOnlineSessionDungeons::HandleSessionCreated(uint64_t uXUID, TOptional<XblGuid> correlationId)
{
	if (uXUID == 0)
	{
		OnSessionCreatedWithFailure();
		return;
	}	
	
	if (!HostedSession.IsValid())
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] Error, no session available when handling 'User_Hadded' event"));
		return;
	}

	FOnlineIdentityDungeons* identityInterface = GetIdentityInterface();

	if (!identityInterface)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] Unable to get IdentityInterface (nullptr)"));
		return;
	}

	if (!identityInterface->GetUserFromXuid(uXUID))
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] Error, Unable to retrieve user from XUID")); //TODO proper handling -> Tear down / leave session etc.
		return;
	}

	if (correlationId) {
		auto NewSessionInfo = MakeShared<FOnlineSessionInfoDungeons>(utility::conversions::to_string_t(correlationId.GetValue().value));

		NewSessionInfo->HostAddr = MakeShareable(new FInternetAddrDungeonsWebRTC(uXUID, 0));
		HostedSession->SessionInfo = NewSessionInfo;
		HostedSession->bHosting = true;

		Subsystem->ExecuteNextTick([this]()
		{
			SetIsInSession(true, true);
			sessionProperties::SetIsReady(true);
		});

		RegisterPlayer(HostedSession->SessionName, FUniqueNetIdDungeons(uXUID), false);
		TriggerOnCreateSessionCompleteDelegates(HostedSession->SessionName, true);
	}
	else {
		OnSessionCreatedWithFailure();
	}
}

void FOnlineSessionDungeons::OnSessionCreatedWithFailure()
{
	HostedSession->SessionInfo = {};

	SetIsInSession(false, false);
	TriggerOnCreateSessionCompleteDelegates(HostedSession->SessionName, false);
}

void FOnlineSessionDungeons::HandleHostChanged()
{
	UE_LOG_ONLINE(Log, TEXT("[Session] The session host has changed"));
	TriggerOnSessionFailureDelegates(FUniqueNetIdString("DisconnectedFromHost"), ESessionFailure::ServiceConnectionLost);
}

void FOnlineSessionDungeons::HandleJoinSessionComplete(FOnlineClientSession::EJoinResult result)
{
	if (!HostedSession.IsValid())
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] Warning, no session available when handling 'JoinLobby' event"));
		return;
	}

	if (result == FOnlineClientSession::EJoinResult::Success)
	{
		UE_LOG_ONLINE(Log, TEXT("[Session] Join lobby succeeded"));
		SetIsInSession(true, false);

		/* Create a task that waits for XMPP to signin before triggering JoinSessionComplete */

		const auto OnCompleteCallback = [this](bool Success, FName SessionName)
		{
			if (Success)
			{
				TriggerOnJoinSessionCompleteDelegates(SessionName, EOnJoinSessionCompleteResult::Success);
			}
			else
			{
				SetIsInSession(false, false);
				TriggerOnJoinSessionCompleteDelegates(SessionName, EOnJoinSessionCompleteResult::UnknownError);
			}
		};

		auto task = MakeUnique<OnlineAsyncTaskDungeonsTriggerOnJoinSessionComplete>(GetIdentityInterface(), OnCompleteCallback, HostedSession->SessionName);
		Subsystem->QueueAsyncTask(std::move(task));
	}
	else
	{
		auto sessionName = HostedSession ? HostedSession->SessionName : FName("");
		if (result == FOnlineClientSession::EJoinResult::NotFound)
		{
			UE_LOG_ONLINE(Warning, TEXT("[Session] 404 - Session not found. Handle, SCID, or Session template cannot be found"));
			TriggerOnJoinSessionCompleteDelegates(sessionName, EOnJoinSessionCompleteResult::SessionDoesNotExist);
		}
		else if (result == FOnlineClientSession::EJoinResult::SessionIsFull)
		{
			TriggerOnJoinSessionCompleteDelegates(sessionName, EOnJoinSessionCompleteResult::SessionIsFull);
		}
		else
		{
			TriggerOnJoinSessionCompleteDelegates(sessionName, EOnJoinSessionCompleteResult::UnknownError);
		}
		// D11.AH - Seeing a crash here after exiting SP game so a NULL check
		if (HostedSession)
		{
			RemoveNamedSession(HostedSession->SessionName);
		}
	}
}

void FOnlineSessionDungeons::HandleClientDisconnectedFromService()
{
	UE_LOG_ONLINE(Log, TEXT("[Session] Disconnected from XSAPI service. Stopping session."));
	TriggerOnSessionFailureDelegates(FUniqueNetIdString("NotConnectedToOnlineService"), ESessionFailure::ServiceConnectionLost);
}

FOnlineIdentityDungeons* FOnlineSessionDungeons::GetIdentityInterface() const
{
	return Subsystem->GetDungeonsIdentityInterface();
}

void FOnlineSessionDungeons::SetIsInSession(bool IsInSession, bool IsHost)
{
	GameState = IsInSession ? SessionState::IN_SESSION : SessionState::DEFAULT;
	if (const auto p2p = GetIdentityInterface()->GetPeer2PeerManager()) {
		p2p->SetIsInSession(IsInSession, IsHost);
		
		SetPlayerActivity();

	}
}

void FOnlineSessionDungeons::SetPlayerActivity()
{
	auto XBLcontext = GetIdentityInterface()->GetXBLContextHandle(GetIdentityInterface()->GetFirstLocalUserNumber());
	if (HostedSession)
	{
		XblMultiplayerSessionReference LobbySession;
		if (HrSuccess(XblMultiplayerManagerLobbySessionSessionReference(&LobbySession), "[Session] Getting session reference (wanting to set an activity)"))
		{
			if (!XblMultiplayerSessionReferenceIsValid(&LobbySession))
			{
				TriggerOnSessionFailureDelegates(FUniqueNetIdString("NotConnectedToOnlineService"), ESessionFailure::ServiceConnectionLost);
				return;
			}
			// D11.PC Comparing if the level is squid coast or not. Maybe we have a better way to do this?
			const bool shouldAdvertise = HostedSession->SessionSettings.bShouldAdvertise && !IsCurrentSessionTutorial();

			dungeonsSession::local::SetActivity(Subsystem, XBLcontext, &LobbySession, shouldAdvertise);

		}
	}
}

bool FOnlineSessionDungeons::IsCurrentSessionTutorial()
{
	return HostedSession->SessionSettings.Settings[OnlineSubsystemConstants::SETTING_LEVEL_NAME.c_str()] == FString("squidcoast");
}

FOnlineSessionDungeons::FOnlineSessionDungeons(FOnlineSubsystemDungeons* InSubsystem)
	: Subsystem(InSubsystem)
	, GameState(SessionState::DEFAULT)
	, CurrentSessionSearch(nullptr)
{
	HrCheck(XblMultiplayerManagerInitialize(OnlineSubsystemConstants::DungeonsLobbySessionName.c_str(), Subsystem->GetQueueHandle()), "[Session] Resetting multiplayer manager at init");
}

FOnlineSessionDungeons::~FOnlineSessionDungeons()
{
	if (GameSession)
	{
		GameSession = {};
		bool done = false;

		//If shutting down Dungeons while in session we need to trigger DoWork X times
		//to make sure we leave the xsapi session
		for (int i = 0; i < 5 && !done; i++)
		{

			dungeonsSession::DoWork([&done](const XblMultiplayerEvent& mpEvent)
			{
				if (mpEvent.EventType == XblMultiplayerEventType::UserRemoved || mpEvent.EventType == XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService)
				{
					done = true;
				}
			});

			using namespace std::literals::chrono_literals;
			std::this_thread::sleep_for(100ms); //Sleep for a maximum of 500 msec
		}
	}
}

bool FOnlineSessionDungeons::Tick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Session_Interface);
	if (GameSession)
	{
		GameSession->Tick(DeltaTime);
	}

	dungeonsSession::DoWork([this](const XblMultiplayerEvent& mpEvent)
	{
		if (FAILED(mpEvent.Result)) {
			GetIdentityInterface()->SetDirtyServiceToken(true);
		}

		if (GameSession) {
			GameSession->HandleEvent(mpEvent);
		}
	});

	PlatformTick(DeltaTime);

	return true;
}

class FNamedOnlineSession* FOnlineSessionDungeons::AddNamedSession(FName SessionName, const FOnlineSessionSettings& SessionSettings) {
	FScopeLock ScopeLock(&SessionLock);
	HostedSession = MakeShared<FNamedOnlineSession>(SessionName, SessionSettings);
	return HostedSession.Get();
}
class FNamedOnlineSession* FOnlineSessionDungeons::AddNamedSession(FName SessionName, const FOnlineSession& Session) {
	FScopeLock ScopeLock(&SessionLock);
	HostedSession = MakeShared<FNamedOnlineSession>(SessionName, Session);
	return HostedSession.Get();
}

TSharedPtr<const FUniqueNetId> FOnlineSessionDungeons::CreateSessionIdFromString(const FString& SessionIdStr) { return nullptr; }

class FNamedOnlineSession* FOnlineSessionDungeons::GetNamedSession(FName SessionName) {
	FScopeLock ScopeLock(&SessionLock);
	return HostedSession.IsValid() && HostedSession->SessionName == SessionName ? &(*HostedSession) : nullptr;
}

void FOnlineSessionDungeons::RemoveNamedSession(FName SessionName)
{
	FScopeLock ScopeLock(&SessionLock);
	HostedSession = {};
}

bool FOnlineSessionDungeons::HasPresenceSession() { return false; }

EOnlineSessionState::Type FOnlineSessionDungeons::GetSessionState(FName SessionName) const {
	FScopeLock ScopeLock(&SessionLock);
	return HostedSession.IsValid() ? HostedSession->SessionState : EOnlineSessionState::NoSession;
}


//IOnlineSession
bool FOnlineSessionDungeons::CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings) {
	bool result = false;

	dungeonsSession::local::InvalidateAdvertiseCache();

	if (GameState == SessionState::CREATING_SESSION)
	{
		UE_LOG_ONLINE(Log, TEXT("[Session] CreateSession already initiated"));
		return false;
	}

	FNamedOnlineSession* ExistingSession = GetNamedSession(SessionName);

	FOnlineIdentityDungeons* identityInterface = GetIdentityInterface();
	const auto user = identityInterface->GetUserFromLocalUser(HostingPlayerNum);

	if (!ExistingSession && user)
	{
		GameState = SessionState::CREATING_SESSION;
		FNamedOnlineSession* Session = AddNamedSession(SessionName, NewSessionSettings);
		check(Session);
		Session->SessionState = EOnlineSessionState::Creating;
		Session->NumOpenPrivateConnections = NewSessionSettings.NumPrivateConnections;
		Session->NumOpenPublicConnections = NewSessionSettings.bIsDedicated ? NewSessionSettings.NumPublicConnections : NewSessionSettings.NumPublicConnections - 1;
		Session->HostingPlayerNum = HostingPlayerNum;
		Session->OwningUserId = user->UniqueNetId;
		Session->OwningUserName = user->GetGamerTag();
		Session->SessionSettings.BuildUniqueId = GetBuildUniqueId();

		GameSession = FOnlineHostedSession::CreateSession(
			GetIdentityInterface(),
			HostingPlayerNum,
			HostedSession,
			Subsystem->GetQueueHandle(),
			[this](uint64_t uXUID, TOptional<XblGuid> correlationId) { HandleSessionCreated(uXUID, correlationId); },
			[this] { HandleClientDisconnectedFromService(); },
			[this](const FUniqueNetId& XUID, ESessionFailure::Type Type) { TriggerOnSessionFailureDelegates(XUID, Type); }
		);
		result = (GameSession != nullptr);
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("Cannot create session '%s': session already exists"), *SessionName.ToString());
	}

	return result;
}

bool FOnlineSessionDungeons::CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings) {
	if (const auto user = GetIdentityInterface()->GetUserFromNetId(HostingPlayerId))
	{
		return CreateSession(user->LocalUserNum, SessionName, NewSessionSettings);
	}

	UE_LOG_ONLINE(Warning, TEXT("[Session] cannot get local user, creating session failed"));
	return false;
}
bool FOnlineSessionDungeons::StartSession(FName SessionName) {
	TriggerOnStartSessionCompleteDelegates(SessionName, true);
	return true;
}

bool FOnlineSessionDungeons::UpdateSession(FName SessionName, FOnlineSessionSettings& UpdatedSessionSettings, bool bShouldRefreshOnlineData) {
	FNamedOnlineSession *session = GetNamedSession(SessionName);

	if (session)
	{
		session->SessionSettings = UpdatedSessionSettings;
		if (GameSession)
		{
			sessionProperties::SetProperties(session);

			SetPlayerActivity();
		}
	}

	return true;
}

bool FOnlineSessionDungeons::EndSession(FName SessionName) {
	return false;
}

bool FOnlineSessionDungeons::DestroySession(FName SessionName, const FOnDestroySessionCompleteDelegate& CompletionDelegate) {
	uint32 Result = ONLINE_FAIL;
	FNamedOnlineSession* Session = GetNamedSession(SessionName);

	if (Session)
	{
		if (Session->SessionState != EOnlineSessionState::Destroying)
		{
			Session->SessionState = EOnlineSessionState::Destroying;
			UnregisterAllPlayers(SessionName);
			GameSession = {};

			RemoveNamedSession(SessionName);
			SetIsInSession(false, false);

			CompletionDelegate.ExecuteIfBound(SessionName, true);
			TriggerOnDestroySessionCompleteDelegates(SessionName, true);
		}
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] Unable to destroy a nulll online session (%s)"), *SessionName.ToString());
	}
	return true;
}

bool FOnlineSessionDungeons::IsPlayerInSession(FName SessionName, const FUniqueNetId& UniqueId) {
	bool bFound = false;
	FNamedOnlineSession* Session = GetNamedSession(SessionName);

	if (Session)
	{
		const bool bIsSessionOwner = *Session->OwningUserId == UniqueId;

		FUniqueNetIdMatcher PlayerMatch(UniqueId);
		if (bIsSessionOwner || Session->RegisteredPlayers.IndexOfByPredicate(PlayerMatch) != INDEX_NONE)
		{
			bFound = true;
		}
	}
	return bFound;
}

bool FOnlineSessionDungeons::StartMatchmaking(const TArray< TSharedRef<const FUniqueNetId> >& LocalPlayers, FName SessionName, const FOnlineSessionSettings& NewSessionSettings, TSharedRef<FOnlineSessionSearch>& SearchSettings) {
	return false;
}

bool FOnlineSessionDungeons::CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName) {
	return false;
}

bool FOnlineSessionDungeons::CancelMatchmaking(const FUniqueNetId& SearchingPlayerId, FName SessionName) {
	return false;
}
#ifdef SUBSYSTEM_HAS_FINDSESSION_CHECK
bool FOnlineSessionDungeons::FindSessionIsActive() const {
	return CurrentSessionSearch.IsValid() ? CurrentSessionSearch->SearchState == EOnlineAsyncTaskState::InProgress : false;
}
#endif
bool FOnlineSessionDungeons::FindSessions(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch>& SearchSettings) {

	uint32 Return = E_FAIL;

	if (!CurrentSessionSearch.IsValid() || CurrentSessionSearch->SearchState != EOnlineAsyncTaskState::InProgress)
	{
		FOnlineIdentityDungeons* IdentityInterface = GetIdentityInterface();

		if (!IdentityInterface)
		{
			UE_LOG_ONLINE(Warning, TEXT("[Session] Failed to find sessions - Identity = nullptr "));
			return false;
		}

		/* Clear local sessions */
		SearchSettings->SearchResults.Empty();
		CurrentSessionSearch = SearchSettings;
		UE_LOG_ONLINE(Log, TEXT("[Session] Triggering find sessions"))
			auto FindXboxLobbies = MakeUnique<OnlineAsyncTaskDungeonsFindLobbies>(Subsystem, CurrentSessionSearch, SearchingPlayerNum);
		Subsystem->QueueAsyncTask(std::move(FindXboxLobbies));
		return true;
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] FindSession already in progress"));
		return false;
	}
}

bool FOnlineSessionDungeons::FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings) {	
	if (const auto user = GetIdentityInterface()->GetUserFromNetId(SearchingPlayerId))
	{
		return FindSessions(user->LocalUserNum, SearchSettings);
	}

	UE_LOG_ONLINE(Warning, TEXT("[Session] cannot get local user, finding sessions failed"));
	return false;
}

bool FOnlineSessionDungeons::FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId, const FUniqueNetId& FriendId, const FOnSingleSessionResultCompleteDelegate& CompletionDelegate) {
	return true;
}

bool FOnlineSessionDungeons::CancelFindSessions() {
	return false;
}

bool FOnlineSessionDungeons::PingSearchResults(const FOnlineSessionSearchResult& SearchResult) {
	return false;
}

/** Get a resolved connection string from a session info */
static bool GetConnectStringFromSessionInfo(TSharedPtr<FOnlineSessionInfoDungeons>& SessionInfo, FString& ConnectInfo, int32 PortOverride = 0)
{
	bool bSuccess = false;
	if (SessionInfo.IsValid())
	{
		if (SessionInfo->HostAddr.IsValid() && SessionInfo->HostAddr->IsValid())
		{
			ConnectInfo = FString::Printf(TEXT("WebRTC.%s"), *SessionInfo->HostAddr->ToString(true));
			bSuccess = true;
		}
	}
	return bSuccess;
}

bool FOnlineSessionDungeons::JoinSession(int32 PlayerNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession) {
	dungeonsSession::local::InvalidateAdvertiseCache();

	uint32 Result = E_FAIL;
	FNamedOnlineSession* Session = GetNamedSession(SessionName);

	if (!Session)
	{
		Session = AddNamedSession(SessionName, DesiredSession.Session);
		Session->HostingPlayerNum = PlayerNum;

		Session->SessionInfo = MakeShared<FOnlineSessionInfoDungeons>(DesiredSession.GetSessionIdStr());

		Result = JoinSessionInternal(PlayerNum, Session, &DesiredSession.Session, SessionName);

		if (Result != ONLINE_SUCCESS)
		{
			UE_LOG_ONLINE(Warning, TEXT("[Session] Failed to join session "));
			Subsystem->QueueAsyncTask([this, SessionName] {
				TriggerOnJoinSessionCompleteDelegates(SessionName, EOnJoinSessionCompleteResult::UnknownError);
			});

			RemoveNamedSession(SessionName);
		}
	}
	else
	{
		UE_LOG_ONLINE(Warning, TEXT("Session (%s) already exists, can't join twice"), *SessionName.ToString());
	}

	return Result == ONLINE_SUCCESS;
}

bool FOnlineSessionDungeons::JoinSession(const FUniqueNetId& PlayerId, FName SessionName, const FOnlineSessionSearchResult& DesiredSession) {
	if (const auto user = GetIdentityInterface()->GetUserFromNetId(PlayerId))
	{
		return JoinSession(user->LocalUserNum, SessionName, DesiredSession);
	}

	UE_LOG_ONLINE(Warning, TEXT("[Session] cannot get local user, joining session failed"));
	return false;
}

bool FOnlineSessionDungeons::FindFriendSession(int32 LocalUserNum, const FUniqueNetId& Friend) {
	return false;
}

bool FOnlineSessionDungeons::FindFriendSession(const FUniqueNetId& LocalUserId, const FUniqueNetId& Friend) {
	return false;
}

bool FOnlineSessionDungeons::FindFriendSession(const FUniqueNetId& LocalUserId, const TArray<TSharedRef<const FUniqueNetId>>& FriendList) {
	return false;
}

bool FOnlineSessionDungeons::SendSessionInviteToFriend(int32 LocalUserNum, FName SessionName, const FUniqueNetId& Friend) {
	const XblUserHandle user = GetIdentityInterface()->GetXBLUserHandle(LocalUserNum);

#if !PLATFORM_SWITCH && !PLATFORM_PS4
	return HrSuccess(XblMultiplayerManagerLobbySessionInviteFriends(user, nullptr, "Join my game!"), "[Session] Sending session invite");
#else // D11.AH 'C' Version - Note option to invite friend is not available on switch need to ask MS
	const FUniqueNetIdDungeons& XBLFriend = static_cast<const FUniqueNetIdDungeons&>(Friend);
	uint64_t XUID = XBLFriend.XUID;
	HRESULT hr = XblMultiplayerManagerLobbySessionInviteUsers(user, &XUID, 1, nullptr, "Join my game!");
	UE_LOG_ONLINE(Log, TEXT("[Session] %s"), *ConvertHRtoString(hr));
	return false;
#endif
}

bool FOnlineSessionDungeons::SendSessionInviteToFriend(const FUniqueNetId& LocalUserId, FName SessionName, const FUniqueNetId& Friend) {
	FOnlineIdentityDungeons* identityInterface = GetIdentityInterface();
	if (!identityInterface)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] Unable to get identity interface (nullptr)"));
		return false;
	}

	const auto user = identityInterface->GetUserFromNetId(LocalUserId);
	if (!user)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] cannot get local XBL user handle, invite failed"));
		return false;
	}

	const FUniqueNetIdDungeons& XBLFriend = static_cast<const FUniqueNetIdDungeons&>(Friend);
	uint64_t XUID = XBLFriend.XUID;
	return HrSuccess(XblMultiplayerManagerLobbySessionInviteUsers(user->XBLUser, &XUID, 1, nullptr, "Join my game!"), "[Session] Sending invite to user");
}

bool FOnlineSessionDungeons::SendSessionInviteToFriends(int32 LocalUserNum, FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Friends) {
	return false;
}

bool FOnlineSessionDungeons::SendSessionInviteToFriends(const FUniqueNetId& LocalUserId, FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Friends) {
	return false;
}

bool FOnlineSessionDungeons::GetResolvedConnectString(FName SessionName, FString& ConnectInfo, FName PortType) {

	bool bSuccess = false;

	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	TSharedPtr<FOnlineSessionInfoDungeons> SessionInfo = StaticCastSharedPtr<FOnlineSessionInfoDungeons>(Session->SessionInfo);
	if (PortType == GamePort)
	{
		bSuccess = GetConnectStringFromSessionInfo(SessionInfo, ConnectInfo);
	}

	return bSuccess;
}

bool FOnlineSessionDungeons::GetResolvedConnectString(const FOnlineSessionSearchResult& SearchResult, FName PortType, FString& ConnectInfo) {
	return false;
}

FOnlineSessionSettings* FOnlineSessionDungeons::GetSessionSettings(FName SessionName) {
	FScopeLock ScopeLock(&SessionLock);
	return HostedSession.IsUnique() ? &HostedSession->SessionSettings : nullptr;
}

bool FOnlineSessionDungeons::RegisterPlayer(FName SessionName, const FUniqueNetId& PlayerId, bool bWasInvited) {
	TArray< TSharedRef<const FUniqueNetId> > Players;
	Players.Add(MakeShared<FUniqueNetIdDungeons>(PlayerId));
	return RegisterPlayers(SessionName, Players, bWasInvited);
}

bool FOnlineSessionDungeons::RegisterPlayers(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Players, bool bWasInvited) {
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	bool bSuccess = false;
	if (Session) {
		for (const auto& PlayerId : Players)
		{
			if (Session->RegisteredPlayers.IndexOfByPredicate(FUniqueNetIdMatcher(*PlayerId)) == INDEX_NONE)
			{
				Session->RegisteredPlayers.Add(PlayerId);
				bSuccess = true;
				UE_LOG_ONLINE(Log, TEXT("[Session] Adding player %s to session %s"), *PlayerId->ToDebugString(), *SessionName.ToString());
				
				if (Session->SessionSettings.bShouldAdvertise)
					Session->NumOpenPublicConnections = PlatformMaxPlayers - Session->RegisteredPlayers.Num();
				else
					Session->NumOpenPrivateConnections = PlatformMaxPlayers - Session->RegisteredPlayers.Num();
			}
			else
			{
				UE_LOG_ONLINE(Log, TEXT("[Session] Player %s already registered in session %s"), *PlayerId->ToDebugString(), *SessionName.ToString());
			}
		}

	}
	else 
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] Failed to get session '%s' when registering player"), *SessionName.ToString());
	}

	TriggerOnRegisterPlayersCompleteDelegates(SessionName, Players, bSuccess);
	return true;
}

bool FOnlineSessionDungeons::UnregisterPlayer(FName SessionName, const FUniqueNetId& PlayerId) {
	TArray< TSharedRef<const FUniqueNetId> > Players;
	Players.Add(MakeShared<FUniqueNetIdDungeons>(PlayerId));
	return UnregisterPlayers(SessionName, Players);
}

bool FOnlineSessionDungeons::UnregisterPlayers(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Players) {
	FNamedOnlineSession* Session = GetNamedSession(SessionName);
	bool bSuccess = false;
	if (Session) {
		bSuccess = true;
		for (const auto& PlayerId : Players)
		{
			int32 RegistrantIndex = Session->RegisteredPlayers.IndexOfByPredicate(FUniqueNetIdMatcher(*PlayerId));
			if (RegistrantIndex != INDEX_NONE)
			{
				Session->RegisteredPlayers.RemoveAtSwap(RegistrantIndex);
				UE_LOG_ONLINE(Log, TEXT("[Session] Removed player %s from session %s"), *PlayerId->ToDebugString(), *SessionName.ToString());

				if (Session->SessionSettings.bShouldAdvertise)
					Session->NumOpenPublicConnections = PlatformMaxPlayers - Session->RegisteredPlayers.Num();
				else
					Session->NumOpenPrivateConnections = PlatformMaxPlayers - Session->RegisteredPlayers.Num();
			}
			else
			{
				UE_LOG_ONLINE_SESSION(Warning, TEXT("[Session] Player %s is not part of session (%s)"), *PlayerId->ToDebugString(), *SessionName.ToString());
			}
		}
	}
	else 
	{
		UE_LOG_ONLINE_SESSION(Warning, TEXT("[Session] No game present to leave for session (%s)"), *SessionName.ToString());
	}

	TriggerOnUnregisterPlayersCompleteDelegates(SessionName, Players, bSuccess);
	return bSuccess;
}

void FOnlineSessionDungeons::UnregisterAllPlayers(FName SessionName) {
	if (FNamedOnlineSession* Session = GetNamedSession(SessionName)) {
		Session->RegisteredPlayers.Empty();
	}
	else {
		UE_LOG_ONLINE_SESSION(Warning, TEXT("[Session] No game session when unregistering players (%s)"), *SessionName.ToString());
	}
}

void FOnlineSessionDungeons::RegisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnRegisterLocalPlayerCompleteDelegate& Delegate) {
	return;
}

void FOnlineSessionDungeons::UnregisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnUnregisterLocalPlayerCompleteDelegate& Delegate) {
	return;
}

int32 FOnlineSessionDungeons::GetNumSessions() {
	return HostedSession ? 1 : 0;
}

void FOnlineSessionDungeons::DumpSessionState() {
	return;
}
