#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionInterface.h"
#include "xsapiServicesInclude.h"

#include "utils.h"
#include "GameSession/OnlineClientSession.h"


class FNboSerializeFromBufferDungeons;
class FNboSerializeToBufferDungeons;
class FOnlineSession;
class FOnlineSessionSettings;
class FOnlineSubsystemDungeons;
class IOnlineGameSession;

/* ENUM class for state machine */
enum class SessionState 
{
	/* Menu */
	DEFAULT,
	/* Searching for sessions*/
	SEARCHING_SESSIONS,
	/* Creating a new session */
	CREATING_SESSION,
	/* In session */
	IN_SESSION,
	/* Currently leaving session */
	LEAVING_SESSION,
	/* Joining session */
	JOINING_SESSION
	/*  */
};

class FOnlineSessionDungeons : public IOnlineSession
{
	uint32 JoinSessionInternal(int32 PlayerNum, FNamedOnlineSession* Session, const FOnlineSession* SearchSession, const FName SessionName);
protected:
	
	/* Handle different tick cases */
	void HandleSessionCreated(uint64_t uXUID, TOptional<XblGuid> correlationId);

	void OnSessionCreatedWithFailure();

	void HandleHostChanged();
	void HandleJoinSessionComplete(FOnlineClientSession::EJoinResult);
	void HandleClientDisconnectedFromService();
	/* --------------------------- */

	/* Util functions */
	/* -------------- */
	class FOnlineIdentityDungeons* GetIdentityInterface() const;
	void SetIsInSession(bool IsInSession, bool IsHost);

	void SetPlayerActivity();
	bool IsCurrentSessionTutorial();

	/** Reference to the main Dungeons subsystem */
	class FOnlineSubsystemDungeons* Subsystem;

	TSharedPtr<FNamedOnlineSession> HostedSession;

	SessionState GameState;

	TUniquePtr<IOnlineGameSession> GameSession;

public:

	/** Current search object */
	TSharedPtr<FOnlineSessionSearch> CurrentSessionSearch;
	mutable FCriticalSection SessionLock;
	
	/* Methods */
	FOnlineSessionDungeons(FOnlineSubsystemDungeons* InSubsystem);

	virtual ~FOnlineSessionDungeons();

	bool Tick(float DeltaTime);
	virtual void PlatformTick(float DeltaTime) {}

	virtual class FNamedOnlineSession* AddNamedSession(FName SessionName, const FOnlineSessionSettings& SessionSettings) override;
	virtual class FNamedOnlineSession* AddNamedSession(FName SessionName, const FOnlineSession& Session) override;

	virtual TSharedPtr<const FUniqueNetId> CreateSessionIdFromString(const FString& SessionIdStr) override;
	virtual class FNamedOnlineSession* GetNamedSession(FName SessionName) override;
	virtual void RemoveNamedSession(FName SessionName) override;
	virtual bool HasPresenceSession() override;
	virtual EOnlineSessionState::Type GetSessionState(FName SessionName) const override;

	//IOnlineSession
	virtual bool CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings) override;
	virtual bool CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings) override;
	virtual bool StartSession(FName SessionName) override;
	virtual bool UpdateSession(FName SessionName, FOnlineSessionSettings& UpdatedSessionSettings, bool bShouldRefreshOnlineData = true) override;
	virtual bool EndSession(FName SessionName) override;
	virtual bool DestroySession(FName SessionName, const FOnDestroySessionCompleteDelegate& CompletionDelegate = FOnDestroySessionCompleteDelegate()) override;
	virtual bool IsPlayerInSession(FName SessionName, const FUniqueNetId& UniqueId) override;
	virtual bool StartMatchmaking(const TArray< TSharedRef<const FUniqueNetId> >& LocalPlayers, FName SessionName, const FOnlineSessionSettings& NewSessionSettings, TSharedRef<FOnlineSessionSearch>& SearchSettings) override;
	virtual bool CancelMatchmaking(int32 SearchingPlayerNum, FName SessionName) override;
	virtual bool CancelMatchmaking(const FUniqueNetId& SearchingPlayerId, FName SessionName) override;
#ifdef SUBSYSTEM_HAS_FINDSESSION_CHECK
	virtual bool FindSessionIsActive() const override;
#endif
	virtual bool FindSessions(int32 SearchingPlayerNum, const TSharedRef<FOnlineSessionSearch>& SearchSettings) override;
	virtual bool FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings) override;
	virtual bool FindSessionById(const FUniqueNetId& SearchingUserId, const FUniqueNetId& SessionId, const FUniqueNetId& FriendId, const FOnSingleSessionResultCompleteDelegate& CompletionDelegate) override;
	virtual bool CancelFindSessions() override;
	virtual bool PingSearchResults(const FOnlineSessionSearchResult& SearchResult) override;
	virtual bool JoinSession(int32 PlayerNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession) override;
	virtual bool JoinSession(const FUniqueNetId& PlayerId, FName SessionName, const FOnlineSessionSearchResult& DesiredSession) override;
	virtual bool FindFriendSession(int32 LocalUserNum, const FUniqueNetId& Friend) override;
	virtual bool FindFriendSession(const FUniqueNetId& LocalUserId, const FUniqueNetId& Friend) override;
	virtual bool FindFriendSession(const FUniqueNetId& LocalUserId, const TArray<TSharedRef<const FUniqueNetId>>& FriendList) override;
	virtual bool SendSessionInviteToFriend(int32 LocalUserNum, FName SessionName, const FUniqueNetId& Friend) override;
	virtual bool SendSessionInviteToFriend(const FUniqueNetId& LocalUserId, FName SessionName, const FUniqueNetId& Friend) override;
	virtual bool SendSessionInviteToFriends(int32 LocalUserNum, FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Friends) override;
	virtual bool SendSessionInviteToFriends(const FUniqueNetId& LocalUserId, FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Friends) override;
	virtual bool GetResolvedConnectString(FName SessionName, FString& ConnectInfo, FName PortType) override;
	virtual bool GetResolvedConnectString(const FOnlineSessionSearchResult& SearchResult, FName PortType, FString& ConnectInfo) override;
	virtual FOnlineSessionSettings* GetSessionSettings(FName SessionName) override;
	virtual bool RegisterPlayer(FName SessionName, const FUniqueNetId& PlayerId, bool bWasInvited) override;
	virtual bool RegisterPlayers(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Players, bool bWasInvited = false) override;
	virtual bool UnregisterPlayer(FName SessionName, const FUniqueNetId& PlayerId) override;
	virtual bool UnregisterPlayers(FName SessionName, const TArray< TSharedRef<const FUniqueNetId> >& Players) override;
	virtual void RegisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnRegisterLocalPlayerCompleteDelegate& Delegate) override;
	virtual void UnregisterLocalPlayer(const FUniqueNetId& PlayerId, FName SessionName, const FOnUnregisterLocalPlayerCompleteDelegate& Delegate) override;
	virtual int32 GetNumSessions() override;
	virtual void DumpSessionState() override;

	void UnregisterAllPlayers(FName SessionName);
};

typedef TSharedPtr<FOnlineSessionDungeons, ESPMode::ThreadSafe> FOnlineSessionDungeonsPtr;

namespace dungeonsSession
{
	namespace platform
	{
		FOnlineSessionDungeonsPtr Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem);
	}
};