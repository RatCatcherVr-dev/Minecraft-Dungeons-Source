#include "Session.h"
#include <algorithm>
#include "util/Algo.hpp"
#include "online/sessions/OnlineUtil.h"
#include "online/crossplay/CrossplayOSS.h"
#include "OnlineSessionInterface.h"

namespace DelegateKeys {
	const FString OnFindFriendSessionComplete = "OnFindFriendSessionCompleteDelegate";
	const FString OnSessionInviteReceived = "OnSessionInviteReceivedDelegate";
	const FString OnSessionFailure = "OnSessionFailureDelegate";
	const FString OnSessionUserInviteAccepted = "OnSessionUserInviteAcceptedDelegate";
	const FString OnFindSessionComplete = "OnFindSessionsCompleteDelegate";
	const FString OnJoinSessionComplete = "OnJoinSessionCompleteDelegate";
}

namespace online {
namespace Crossplay {
Session::Session(const SubsystemRepo& subsystems) : SubOSS(subsystems){ }

FNamedOnlineSession* Session::GetNamedSession(FName SessionName) {
	return SessionType.IsSet() ? GetSubsystemFromName(SessionType.GetValue())->GetSessionInterface()->GetNamedSession(SessionName) : nullptr;
}

//Assumption: If Crossplay enabled PS4 FUniqueNetId will always return false
bool Session::IsPlayerInSession(FName SessionName, const FUniqueNetId& UniqueId) {
	if (const auto* subsystem = GetSubsystemFromUniqueNetId(UniqueId)) {
		return subsystem->GetSessionInterface()->IsPlayerInSession(SessionName, UniqueId);
	}
	check(!"IsPlayerInSession : No subsystem associated with the unique net ID.");
	UE_LOG(LogOnline, Warning, TEXT("[Session] IsPlayerInSession with a subsystem type that is not loaded (type='%s')"), *UniqueId.GetType().ToString());
	return false;
}

void Session::ClearOnFindFriendSessionCompleteDelegate_Handle(int localUserNum, FDelegateHandle& Handle) {
	ClearDelegateHandles(DelegateKeys::OnFindFriendSessionComplete, [localUserNum](TSharedRef<Child<FDelegateHandle>>& child){
		child->Subsystem->GetSessionInterface()->ClearOnFindFriendSessionCompleteDelegate_Handle(localUserNum, child->obj);
	});
}

FDelegateHandle Session::AddOnFindFriendSessionCompleteDelegate_Handle(int localUserNum, const FOnFindFriendSessionCompleteDelegate& Delegate) {
	const auto subsystems = GetSubsystemsWithSessionInterface();
	FDelegateHandle handle(FDelegateHandle::GenerateNewHandle);
	auto sharedLink = MakeShared<Link<FDelegateHandle>>(&handle, subsystems.Num());

	for (const auto& subsystem : subsystems)
	{
		FOnFindFriendSessionCompleteDelegate internalDelegate;
		auto sharedChild = MakeShared<Child<FDelegateHandle>>(subsystem, FDelegateHandle());
		internalDelegate.BindLambda([Delegate, sharedLink, sharedChild, localUserNum](int32 i, bool b, const TArray<FOnlineSessionSearchResult>& results) {
			sharedChild->Success = b;

			//#TODO : Merge results array before calling delegate.
			if (--sharedLink->ChildrenNotReturned > 0) {
				return;
			}

			bool success = algo::all_of(sharedLink->Children, RETLAMBDA(it->Success));

			Delegate.ExecuteIfBound(i, success, results);
		});

		sharedChild->obj = subsystem->GetSessionInterface()->AddOnFindFriendSessionCompleteDelegate_Handle(localUserNum, internalDelegate);
		if (sharedChild->obj.IsValid()) {
			sharedLink->Children.Add(sharedChild);
		}
		else {
			handle.Reset();
		}
	}

	AddDelegateHandle(DelegateKeys::OnFindFriendSessionComplete, sharedLink);
	return handle;
}

//This seems to be called from within the sub systems? Need to make sure we pass the Friend into the correct subsystem.
bool Session::FindFriendSession(int32 LocalUserNum, const FUniqueNetId& Friend) {
	if (const auto* subsystem = GetSubsystemFromUniqueNetId(Friend)) {
		return subsystem->GetSessionInterface()->FindFriendSession(LocalUserNum, Friend);
	}
	check(!"FindFriendSession : No subsystem associated with the unique net ID.");
	UE_LOG(LogOnline, Warning, TEXT("[Session] IsPlayerInSession with a subsystem type that is not loaded (type='%s')"), *Friend.GetType().ToString());
	return false;
}

bool Session::CreateSession(int32 HostingPlayerNum, FName SessionName, const FOnlineSessionSettings& NewSessionSettings) {
	SessionType = GetSessionSubsystem()->GetSubsystemName();
	getCrossplayOss()->SetNetDriver(Subsystems.GetSubsystemType(SessionType.Get("")));
	getCrossplayOss()->SetDefaultSubsystem(Subsystems.GetSubsystemType(SessionType.Get("")));
	return GetSessionInterface()->CreateSession(HostingPlayerNum, SessionName, NewSessionSettings);
}

bool Session::CreateSession(const FUniqueNetId& HostingPlayerId, FName SessionName, const FOnlineSessionSettings& NewSessionSettings) {
	check(IsCompatibleNetId(GetSessionSubsystem(), HostingPlayerId));
	SessionType = GetSessionSubsystem()->GetSubsystemName();
	getCrossplayOss()->SetNetDriver(Subsystems.GetSubsystemType(SessionType.Get("")));
	getCrossplayOss()->SetDefaultSubsystem(Subsystems.GetSubsystemType(SessionType.Get("")));
	return GetSessionInterface()->CreateSession(HostingPlayerId, SessionName, NewSessionSettings);
}

void Session::ClearOnCreateSessionCompleteDelegate_Handle(FDelegateHandle& Handle) {
	//We need to loop over all subsystems and clear the delegates as we may have change the crossplay setting
	//and the delegate might live in the other subsystem.
	Subsystems.ForAllSubsystems([&Handle](IOnlineSubsystem* subsystem)	{
		subsystem->GetSessionInterface()->ClearOnCreateSessionCompleteDelegate_Handle(Handle);
	});
}

FDelegateHandle Session::AddOnCreateSessionCompleteDelegate_Handle(const FOnCreateSessionCompleteDelegate& Delegate) {
	return GetSessionInterface()->AddOnCreateSessionCompleteDelegate_Handle(Delegate);
}

bool Session::StartSession(FName SessionName) {
	return GetSessionInterface()->StartSession(SessionName);
}

void Session::ClearOnStartSessionCompleteDelegate_Handle(FDelegateHandle& Handle) {
	//We need to loop over all subsystems and clear the delegates as we may have change the crossplay setting
	//and the delegate might live in the other subsystem.
	Subsystems.ForAllSubsystems([&Handle](IOnlineSubsystem* subsystem) {
		subsystem->GetSessionInterface()->ClearOnStartSessionCompleteDelegate_Handle(Handle);
	});
}

FDelegateHandle Session::AddOnStartSessionCompleteDelegate_Handle(const FOnStartSessionCompleteDelegate& Delegate) {
	return GetSessionInterface()->AddOnStartSessionCompleteDelegate_Handle(Delegate);
}

bool Session::UpdateSession(FName SessionName, FOnlineSessionSettings& UpdatedSessionSettings, bool bShouldRefreshOnlineData) {
	//Assuming this method is only called on host the session type is always dependent on the toggle.
	return GetSessionInterface()->UpdateSession(SessionName, UpdatedSessionSettings, bShouldRefreshOnlineData);
}

bool Session::RegisterPlayer(FName SessionName, const FUniqueNetId& PlayerId, bool bWasInvited) {
	if (const auto* subsystem = Subsystems.GetActive(SubsystemType::Null)) {
		return subsystem->GetSessionInterface()->RegisterPlayer(SessionName, PlayerId, bWasInvited);
	}

	check(SessionType.IsSet() && "SessionType is not set!");
	if (SessionType->IsEqual(FName(TEXT("DUNGEONS")))) {
		return false; // For dungeons OSS we register players based on XSAPI events.
	}
	const auto* subsystem = GetSubsystemFromName(SessionType.GetValue());
	return subsystem->GetSessionInterface()->RegisterPlayer(SessionName, PlayerId, bWasInvited);
}

bool Session::UnregisterPlayer(FName SessionName, const FUniqueNetId& PlayerId) {
	if (const auto* subsystem = Subsystems.GetActive(SubsystemType::Null)) {
		return subsystem->GetSessionInterface()->UnregisterPlayer(SessionName, PlayerId);
	}

	check(SessionType.IsSet() && "SessionType is not set!");
	if (SessionType->IsEqual(FName(TEXT("DUNGEONS")))) {
		return false; // For dungeons OSS we unregister players based on XSAPI events.
	}
	const auto* subsystem = GetSubsystemFromName(SessionType.GetValue());
	return subsystem->GetSessionInterface()->UnregisterPlayer(SessionName, PlayerId);
}

bool Session::DestroySession(FName SessionName) {
	check(SessionType.IsSet() && "SessionType is not set!");
	const auto SessionValue = SessionType.GetValue();
	SessionType.Reset();
	if (online::getCrossplayOss()->IsPS4Active()) {
		GEngine->ShutdownAllNetDrivers();
	}
	return GetSubsystemFromName(SessionValue)->GetSessionInterface()->DestroySession(SessionName);
}

void Session::ClearOnDestroySessionCompleteDelegate_Handle(FDelegateHandle& Handle) {
	//We need to loop over all subsystems and clear the delegates as we may have change the crossplay setting
	//and the delegate might live in the other subsystem.
	Subsystems.ForAllSubsystems([&Handle](IOnlineSubsystem* subsystem) {
		subsystem->GetSessionInterface()->ClearOnDestroySessionCompleteDelegate_Handle(Handle);
	});
}

FDelegateHandle Session::AddOnDestroySessionCompleteDelegate_Handle(const FOnDestroySessionCompleteDelegate& Delegate) {
	if (GetNamedSession(DungeonsGameSessionName)) {
		const auto SessionValue = SessionType.GetValue();
		return GetSubsystemFromName(SessionValue)->GetSessionInterface()->AddOnDestroySessionCompleteDelegate_Handle(Delegate);
	}
	return GetSessionInterface()->AddOnDestroySessionCompleteDelegate_Handle(Delegate);
}

void Session::ClearOnSessionInviteReceivedDelegate_Handle(FDelegateHandle& Handle) {
	ClearDelegateHandles(DelegateKeys::OnSessionInviteReceived, [](TSharedRef<Child<FDelegateHandle>>& child) {
		child->Subsystem->GetSessionInterface()->ClearOnSessionInviteReceivedDelegate_Handle(child->obj);
	});
}

FDelegateHandle Session::AddOnSessionInviteReceivedDelegate_Handle(const FOnSessionInviteReceivedDelegate& Delegate) {
	if (const auto* DungeonsSubsystem = GetDungeonsSubsystem())
	{
		return DungeonsSubsystem->GetSessionInterface()->AddOnSessionInviteReceivedDelegate_Handle(Delegate);
	}
	return FDelegateHandle();
}

void Session::ClearOnSessionFailureDelegate_Handle(FDelegateHandle& Handle) {
	ClearDelegateHandles(DelegateKeys::OnSessionFailure, [](TSharedRef<Child<FDelegateHandle>>& child) {
		child->Subsystem->GetSessionInterface()->ClearOnSessionFailureDelegate_Handle(child->obj);
	});
}

//fired when an unexpected error occurs that impacts session connectivity or use
FDelegateHandle Session::AddOnSessionFailureDelegate_Handle(const FOnSessionFailureDelegate& Delegate) {
	const auto subsystems = GetSubsystemsWithSessionInterface();
	FDelegateHandle handle(FDelegateHandle::GenerateNewHandle);
	auto sharedLink = MakeShared<Link<FDelegateHandle>>(&handle, subsystems.Num());

	for (const auto& subsystem : subsystems)
	{
		FOnSessionFailureDelegate internalDelegate;
		auto sharedChild = MakeShared<Child<FDelegateHandle>>(subsystem, FDelegateHandle());
		internalDelegate.BindLambda([Delegate, sharedLink, sharedChild](const FUniqueNetId& netId, ESessionFailure::Type failureReason) {
			Delegate.ExecuteIfBound( netId, failureReason);
		});

		sharedChild->obj = subsystem->GetSessionInterface()->AddOnSessionFailureDelegate_Handle(internalDelegate);
		if (sharedChild->obj.IsValid()) {
			sharedLink->Children.Add(sharedChild);
		}
		else {
			handle.Reset();
		}
	}

	AddDelegateHandle(DelegateKeys::OnSessionFailure, sharedLink);
	return handle;
}

void Session::ClearOnSessionUserInviteAcceptedDelegate_Handle(FDelegateHandle& Handle) {
	ClearDelegateHandles(DelegateKeys::OnSessionUserInviteAccepted, [](TSharedRef<Child<FDelegateHandle>>& child) {
		child->Subsystem->GetSessionInterface()->ClearOnSessionUserInviteAcceptedDelegate_Handle(child->obj);
	});
}

FDelegateHandle Session::AddOnSessionUserInviteAcceptedDelegate_Handle(const FOnSessionUserInviteAcceptedDelegate& Delegate) {
	const auto subsystems = GetSubsystemsWithSessionInterface();
	FDelegateHandle handle(FDelegateHandle::GenerateNewHandle);
	auto sharedLink = MakeShared<Link<FDelegateHandle>>(&handle, subsystems.Num());

	for (const auto& subsystem : subsystems)
	{
		FOnSessionUserInviteAcceptedDelegate internalDelegate;
		auto sharedChild = MakeShared<Child<FDelegateHandle>>(subsystem, FDelegateHandle());
		internalDelegate.BindLambda([Delegate, sharedLink, sharedChild](const bool b, const int32 i, TSharedPtr<const FUniqueNetId> netID, const FOnlineSessionSearchResult& inviteResult) {
			//TODO: Should we filter out session invtes from non-compatible sessions? I.e. session with different crossplay setting? Or should we show Ui?

			Delegate.ExecuteIfBound(b, i, netID, inviteResult);
		});

		sharedChild->obj = subsystem->GetSessionInterface()->AddOnSessionUserInviteAcceptedDelegate_Handle(internalDelegate);
		if (sharedChild->obj.IsValid()) {
			sharedLink->Children.Add(sharedChild);
		}
		else {
			handle.Reset();
		}
	}

	AddDelegateHandle(DelegateKeys::OnSessionUserInviteAccepted, sharedLink);
	return handle;
}

void Session::ClearOnFindSessionsCompleteDelegate_Handle(FDelegateHandle& Handle) {
	ClearDelegateHandles(DelegateKeys::OnFindSessionComplete, [](TSharedRef<Child<FDelegateHandle>>& child) {
		child->Subsystem->GetSessionInterface()->ClearOnFindSessionsCompleteDelegate_Handle(child->obj);
	});
}

namespace internal {
	bool SortCrossplaySessions(const FOnlineSessionSearchResult& lhs, const FOnlineSessionSearchResult& rhs) {

		if (!lhs.Session.SessionInfo.IsValid()) {
			//Put sessions with invalid pointers last in list -> Should never happend?
			return false;
		}

		if (!rhs.Session.SessionInfo.IsValid()) {
			return true;
		}

		FName lhsType = lhs.Session.SessionInfo->GetSessionId().GetType();
		FName rhsType = rhs.Session.SessionInfo->GetSessionId().GetType();
		if (lhsType == rhsType) {
			return true; //Keep order if comparing PS4 or XSAPI sessions
		}
		else {
			return lhsType == PS4_SUBSYSTEM && rhsType == TEXT("DUNGEONS");
		}
	}
}

FDelegateHandle Session::AddOnFindSessionsCompleteDelegate_Handle(const FOnFindSessionsCompleteDelegate& Delegate) {
	const auto subsystems = GetSubsystemsWithSessionInterface();
	FDelegateHandle handle(FDelegateHandle::GenerateNewHandle);
	auto sharedLink = MakeShared<Link<FDelegateHandle>>(&handle, subsystems.Num());
	for (const auto& subsystem : subsystems) {
		FOnFindSessionsCompleteDelegate internalDelegate;
		auto sharedChild = MakeShared<Child<FDelegateHandle>>(subsystem, FDelegateHandle());
		internalDelegate.BindLambda([Delegate, sharedLink, sharedChild, &sessionSearchRef = SessionSearches, &proxySessionSearchRef = ProxySessionSearch, SessionPointer = this](bool bWasSuccessful) {
			sharedChild->Success = bWasSuccessful;

			if (sharedChild->Success)
			{
				auto subsystemName = sharedChild->Subsystem->GetSubsystemName();
				auto subsystemsSessionSearch = sessionSearchRef[subsystemName];
				for (const auto& r : subsystemsSessionSearch->SearchResults)
				{
					proxySessionSearchRef->SearchResults.Add(r);
				}
			}

			if (--sharedLink->ChildrenNotReturned > 0) {
				return;
			}

			bool success = algo::all_of(sharedLink->Children, RETLAMBDA(it->Success));

			proxySessionSearchRef->SearchState = success ? EOnlineAsyncTaskState::Type::Done : EOnlineAsyncTaskState::Type::Failed;
			proxySessionSearchRef->SearchResults.Sort(internal::SortCrossplaySessions);
			Delegate.ExecuteIfBound(success);
		});

		sharedChild->obj = subsystem->GetSessionInterface()->AddOnFindSessionsCompleteDelegate_Handle(internalDelegate);
		if (sharedChild->obj.IsValid()) {
			sharedLink->Children.Add(sharedChild);
		}
		else {
			handle.Reset();
		}
	}
	AddDelegateHandle(DelegateKeys::OnFindSessionComplete, sharedLink);
	return handle;
}

bool Session::FindSessionIsActive() const {
	bool result = false;
#ifdef SUBSYSTEM_HAS_FINDSESSION_CHECK
	for (const auto& subsystem : GetSubsystemsWithSessionInterface()) {
		result |= subsystem->GetSessionInterface()->FindSessionIsActive();
	}
#endif
	return result;
}

bool Session::FindSessions(const FUniqueNetId& SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings) {
	ProxySessionSearch = SearchSettings;
	SessionSearches.Empty();
	bool result = true;

	for (const auto& subsystem : GetSubsystemsWithSessionInterface()) {
		FOnlineSessionSearch* sessionSearchCopy = new FOnlineSessionSearch(SearchSettings.Get());
		TSharedPtr<FOnlineSessionSearch> sharedSessionSearchCopy = MakeShareable(sessionSearchCopy);
		SessionSearches.Add(subsystem->GetSubsystemName(), sharedSessionSearchCopy);

		TSharedRef<FOnlineSessionSearch> sharedSessionSearchCopyRef = sharedSessionSearchCopy.ToSharedRef();
		auto subsystemSpecificID = GetUniqueNetIdFromName(SearchingPlayerId, subsystem->GetSubsystemName());
		if (subsystemSpecificID.IsValid())
		{
			result &= subsystem->GetSessionInterface()->FindSessions(*subsystemSpecificID, sharedSessionSearchCopyRef);
		}
		else
		{
			UE_LOG(LogOnline, Warning, TEXT("[Session] FindSessions with a subsystemSpecificID not valid"));
			result = false;
		}
	};

	if (!result)
	{
		ProxySessionSearch->SearchState = EOnlineAsyncTaskState::Type::Failed;
	}
	else
	{
		ProxySessionSearch->SearchState = EOnlineAsyncTaskState::Type::InProgress;
	}

	return result;
}

bool Session::FindSessions(int SearchingPlayerId, const TSharedRef<FOnlineSessionSearch>& SearchSettings) {
	ProxySessionSearch = SearchSettings;
	SessionSearches.Empty();
	bool result = true;

	for (const auto& subsystem : GetSubsystemsWithSessionInterface()) {
		FOnlineSessionSearch* sessionSearchCopy = new FOnlineSessionSearch(SearchSettings.Get());
		TSharedPtr<FOnlineSessionSearch> sessionSearchCopyRef = MakeShareable(sessionSearchCopy);
		SessionSearches.Add(subsystem->GetSubsystemName(), sessionSearchCopyRef);

		TSharedRef<FOnlineSessionSearch> SearchSettingsRef = sessionSearchCopyRef.ToSharedRef();
		result &= subsystem->GetSessionInterface()->FindSessions(SearchingPlayerId, SearchSettingsRef);
	};

	if (!result)
	{
		ProxySessionSearch->SearchState = EOnlineAsyncTaskState::Type::Failed;
	}
	else
	{
		ProxySessionSearch->SearchState = EOnlineAsyncTaskState::Type::InProgress;
	}

	return result;
}

bool Session::SendSessionInviteToFriend(const FUniqueNetId& LocalUserId, FName SessionName, const FUniqueNetId& Friend) {
	const auto* subsystem = GetSessionSubsystem();
	check(IsCompatibleNetId(subsystem, LocalUserId) && "Sending invite from a non-compatible user should not be allowed");
	check(IsCompatibleNetId(subsystem, Friend) && "Sending invite to a non-compatible user should not be allowed");
	return GetSessionInterface()->SendSessionInviteToFriend(LocalUserId, SessionName, Friend);
}

bool Session::SendSessionInviteToFriend(int LocalUserNum, FName SessionName, const FUniqueNetId& Friend) {
	const auto* subsystem = GetSessionSubsystem();
	check(IsCompatibleNetId(subsystem, Friend) && "Sending invite to a non-compatible user should not be allowed");
	return subsystem ? subsystem->GetSessionInterface()->SendSessionInviteToFriend(LocalUserNum, SessionName, Friend) : false;
}

void Session::ClearOnJoinSessionCompleteDelegate_Handle(FDelegateHandle& Handle) {
	ClearDelegateHandles(DelegateKeys::OnJoinSessionComplete, [](TSharedRef<Child<FDelegateHandle>>& child) {
		child->Subsystem->GetSessionInterface()->ClearOnJoinSessionCompleteDelegate_Handle(child->obj);
	});
}

FDelegateHandle Session::AddOnJoinSessionCompleteDelegate_Handle(const FOnJoinSessionCompleteDelegate& Delegate) {
	const auto subsystems = GetSubsystemsWithSessionInterface();
	FDelegateHandle handle(FDelegateHandle::GenerateNewHandle);
	auto sharedLink = MakeShared<Link<FDelegateHandle>>(&handle, subsystems.Num());

	for (const auto& subsystem : subsystems) {
		FOnJoinSessionCompleteDelegate internalDelegate;
		auto sharedChild = MakeShared<Child<FDelegateHandle>>(subsystem, FDelegateHandle());
		internalDelegate.BindLambda([Delegate, sharedLink, sharedChild](FName Sessionname, EOnJoinSessionCompleteResult::Type failureReason) {
			Delegate.ExecuteIfBound(Sessionname, failureReason);
		});

		sharedChild->obj = subsystem->GetSessionInterface()->AddOnJoinSessionCompleteDelegate_Handle(internalDelegate);
		if (sharedChild->obj.IsValid()) {
			sharedLink->Children.Add(sharedChild);
		}
		else {
			handle.Reset();
		}
	}
	AddDelegateHandle(DelegateKeys::OnJoinSessionComplete, sharedLink);
	return handle;
}

bool Session::JoinSession(int32 LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession) {
	const auto* subsystem = GetSubsystemFromUniqueNetId(*DesiredSession.Session.OwningUserId.Get());
	check(subsystem && "subsystem should not be nullptr");
	check((online::IsCrossplayEnabled() || subsystem->GetSubsystemName() != TEXT("DUNGEONS")) && "This combination is not valid!"); //TEMP check during development
	SessionType = subsystem->GetSubsystemName();
	online::getCrossplayOss()->SetNetDriver(Subsystems.GetSubsystemType(SessionType.Get("")));
	online::getCrossplayOss()->SetDefaultSubsystem(Subsystems.GetSubsystemType(SessionType.Get("")));
	return subsystem->GetSessionInterface()->JoinSession(LocalUserNum, SessionName, DesiredSession);
}

bool Session::JoinSession(const FUniqueNetId& LocalUserNum, FName SessionName, const FOnlineSessionSearchResult& DesiredSession) {
	const auto* subsystem = GetSubsystemFromUniqueNetId(*DesiredSession.Session.OwningUserId.Get());
	check(subsystem && "subsystem should not be nullptr");
	check((online::IsCrossplayEnabled() || subsystem->GetSubsystemName() != TEXT("DUNGEONS")) && "This combination is not valid!"); //TEMP check during development
	SessionType = subsystem->GetSubsystemName();
	online::getCrossplayOss()->SetNetDriver(Subsystems.GetSubsystemType(SessionType.Get("")));
	online::getCrossplayOss()->SetDefaultSubsystem(Subsystems.GetSubsystemType(SessionType.Get("")));
	return subsystem->GetSessionInterface()->JoinSession(LocalUserNum, SessionName, DesiredSession);
}

bool Session::GetResolvedConnectString(FName SessionName, FString& ConnectInfo, FName PortType) {
	check(SessionType.IsSet() && "SessionType is not set!");
	const auto* subsystem = GetSubsystemFromName(SessionType.GetValue());
	return subsystem->GetSessionInterface()->GetResolvedConnectString(SessionName, ConnectInfo, PortType);
}

#if defined(SUBSYSTEM_HAS_TELEMETRY_CALLBACKS)
FDelegateHandle Session::AddOnWriteConnectionTypeTelemetryDelegate_Handle(const FOnWriteConnectionTypeTelemetryDelegate& Delegate) {
	const auto* subsystem = Subsystems.Get(SubsystemType::Dungeons);
	return subsystem ? subsystem->GetSessionInterface()->AddOnWriteConnectionTypeTelemetryDelegate_Handle(Delegate) : FDelegateHandle();
}
#endif

void Session::TriggerOnSessionFailureDelegates(FUniqueNetIdString param1, ESessionFailure::Type param2) {
	GetSessionInterface()->TriggerOnSessionFailureDelegates(param1, param2);
}

IOnlineSessionPtr Session::GetFirstActive() const {
	return Subsystems.GetFirstActiveSubsystem()->GetSessionInterface();
}

IOnlineSessionPtr Session::GetSessionInterface() const {
	return GetSessionSubsystem()->GetSessionInterface();
}

TArray<IOnlineSubsystem*> Session::GetSubsystemsWithSessionInterface() const {
	return Subsystems.GetAllActive().FilterByPredicate([](const IOnlineSubsystem* subsystem) {
		return subsystem->GetSessionInterface().IsValid();
	});
}
}
}
