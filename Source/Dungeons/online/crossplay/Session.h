#pragma once
#include "SubOSS.h"

namespace online {
namespace Crossplay {

const FString OnFindFriendSessionComplete = "OnFindFriendSessionCompleteDelegateHandle";

class Session : public SubOSS {
public:
	Session(const SubsystemRepo&);

	FNamedOnlineSession* GetNamedSession(FName SessionName);
	bool IsPlayerInSession(FName SessionName, const FUniqueNetId& UniqueId);
	bool UpdateSession(FName SessionName, FOnlineSessionSettings& UpdatedSessionSettings, bool bShouldRefreshOnlineData = true);

	bool CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings);
	bool CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings);
	void ClearOnCreateSessionCompleteDelegate_Handle(FDelegateHandle& Handle);
	FDelegateHandle AddOnCreateSessionCompleteDelegate_Handle(const FOnCreateSessionCompleteDelegate& Delegate);

	bool FindFriendSession(int32 LocalUserNum, const FUniqueNetId& Friend);
	void ClearOnFindFriendSessionCompleteDelegate_Handle(int localUserNum, FDelegateHandle& Handle);
	FDelegateHandle AddOnFindFriendSessionCompleteDelegate_Handle(int localUserNum, const FOnFindFriendSessionCompleteDelegate& Delegate);

	bool StartSession(FName SessionName);
	void ClearOnStartSessionCompleteDelegate_Handle(FDelegateHandle& Handle);
	FDelegateHandle AddOnStartSessionCompleteDelegate_Handle(const FOnStartSessionCompleteDelegate& Delegate);

	bool RegisterPlayer(FName SessionName, const FUniqueNetId& PlayerId, bool bWasInvited);
	bool UnregisterPlayer(FName SessionName, const FUniqueNetId& PlayerId);

	bool DestroySession(FName SessionName);
	void ClearOnDestroySessionCompleteDelegate_Handle(FDelegateHandle& Handle);
	FDelegateHandle AddOnDestroySessionCompleteDelegate_Handle(const FOnDestroySessionCompleteDelegate& Delegate);

	void ClearOnSessionInviteReceivedDelegate_Handle(FDelegateHandle& Handle);
	FDelegateHandle AddOnSessionInviteReceivedDelegate_Handle(const FOnSessionInviteReceivedDelegate& Delegate);

	void ClearOnSessionFailureDelegate_Handle(FDelegateHandle& Handle);
	FDelegateHandle AddOnSessionFailureDelegate_Handle(const FOnSessionFailureDelegate& Delegate);

	void ClearOnSessionUserInviteAcceptedDelegate_Handle(FDelegateHandle& Handle);
	FDelegateHandle AddOnSessionUserInviteAcceptedDelegate_Handle(const FOnSessionUserInviteAcceptedDelegate& Delegate);

	void ClearOnFindSessionsCompleteDelegate_Handle(FDelegateHandle& Handle);
	FDelegateHandle AddOnFindSessionsCompleteDelegate_Handle(const FOnCancelFindSessionsCompleteDelegate& Delegate);

	bool FindSessionIsActive() const;
	bool FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings);
	bool FindSessions(int SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings);

	bool SendSessionInviteToFriend(const FUniqueNetId& LocalUserNum, FName SessionName, const FUniqueNetId& Friend);
	bool SendSessionInviteToFriend(int LocalUserNum, FName SessionName, const FUniqueNetId& Friend);

	void ClearOnJoinSessionCompleteDelegate_Handle(FDelegateHandle& Handle);
	FDelegateHandle AddOnJoinSessionCompleteDelegate_Handle(const FOnJoinSessionCompleteDelegate& Delegate);
	bool JoinSession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession);
	bool JoinSession(const FUniqueNetId& LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession);

	bool GetResolvedConnectString(FName SessionName, FString& ConnectInfo, FName PortType = NAME_GamePort);

	void TriggerOnSessionFailureDelegates(FUniqueNetIdString, ESessionFailure::Type);

	TArray<IOnlineSubsystem*> GetSubsystemsWithSessionInterface() const;

#if defined(SUBSYSTEM_HAS_TELEMETRY_CALLBACKS)
	FDelegateHandle AddOnWriteConnectionTypeTelemetryDelegate_Handle(const FOnWriteConnectionTypeTelemetryDelegate& Delegate);
#endif

private:
	IOnlineSessionPtr GetFirstActive() const;
	IOnlineSessionPtr GetSessionInterface() const;

	TSharedPtr<FOnlineSessionSearch> ProxySessionSearch;
	TMap<FName, TSharedPtr<class FOnlineSessionSearch>> SessionSearches;

	TOptional<FName> SessionType;
};
}
}
