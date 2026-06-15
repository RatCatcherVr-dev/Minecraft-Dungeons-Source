#include "PCHOnlineDungeonsSubsystem.h"
#include "AsyncTasks/OnlineAsyncTaskDungeonsQueryLobbies.h"

#include "OnlineSubsystemDungeons.h"
#include "OnlineSessionInterfaceDungeons.h"
#include "OnlineIdentityDungeons.h"
#include "SessionProperties.h"
#include "Private/utils.h"
#include "HrLog.h"

OnlineAsyncTaskDungeonsQueryLobbies::OnlineAsyncTaskDungeonsQueryLobbies(FOnlineSubsystemDungeons* InSubsystem, const TSharedPtr<FOnlineSessionSearch> InSearchSettings, int32 InSearchingPlayerNum, std::vector<XblMultiplayerActivityDetails> InSessions)
	: Subsystem(InSubsystem), SearchSettings(InSearchSettings), SearchingPlayerNum(InSearchingPlayerNum), Sessions(InSessions), PendingCalls(0)
{
	QuerySessions(Sessions);
}

void OnlineAsyncTaskDungeonsQueryLobbies::Tick()
{
	check(PendingCalls >= 0 && "Query session (tick) pending calls out of sync");
	if (PendingCalls <= 0)
	{
		bWasSuccessful = true;
		bIsComplete = true;
	}
}

void OnlineAsyncTaskDungeonsQueryLobbies::Finalize()
{
	FOnlineSessionDungeons* SessionInterface = static_cast<FOnlineSessionDungeons*>(Subsystem->GetSessionInterface().Get());
	if (!SessionInterface)
	{
		UE_LOG_ONLINE(Warning, TEXT("[QueryLobbies] Failed to get session interface (nullptr)"));
	}
	else
	{
		if (SessionInterface->CurrentSessionSearch.IsValid() && SearchSettings == SessionInterface->CurrentSessionSearch)
		{
			SessionInterface->CurrentSessionSearch = nullptr;
		}
	}

	SearchSettings->SearchState = EOnlineAsyncTaskState::Done;
}

void OnlineAsyncTaskDungeonsQueryLobbies::TriggerDelegates()
{
	IOnlineSessionPtr SessionInt = Subsystem->GetSessionInterface();
	if (SessionInt.IsValid())
	{
		SessionInt->TriggerOnFindSessionsCompleteDelegates(bWasSuccessful);
	}
}

/*  Inconsistent member state, which is a result of a force disconnect, i.e. the Ethernet cable was brutally ejected and then inserted back 1 min later.
	Current player which remains in the lobby, but is not in the game.
*/
bool IsCurrentForceDisconnectedPlayer(const XblMultiplayerSessionMember& member)
{
	return member.IsCurrentUser && !member.SecureDeviceBaseAddress64;
}

/*  Checks if the current member list contains force-disconnected players.
	@return true if one has been detected.
*/
bool CheckForceDisconnectedMembers(FOnlineIdentityDungeons* IdentityInterface, const XblMultiplayerSessionHandle& SessionHandle) {
#if PLATFORM_XBOXONE
	size_t MemberCount = 0;
	const XblMultiplayerSessionMember* Members = nullptr;

	if (HrFailed(XblMultiplayerSessionMembers(SessionHandle, &Members, &MemberCount), "[Session] Getting session members"))
		return false;

	for (size_t i = 0; i < MemberCount; ++i) {
		const XblMultiplayerSessionMember& member = Members[i];
		auto Gamertag = FString(ANSI_TO_TCHAR(member.Gamertag));
		/* D11.IvanG - AB#334978, force-disconnected member detected. */
		if (IsCurrentForceDisconnectedPlayer(member)) {
			/*  From XSAPI docs: https://docs.microsoft.com/en-us/gaming/xbox-live/features/multiplayer/mpsd/concepts/live-mpsd-details
				If the user's state is Active on title launch, this situation is probably due to a network disconnect or another scenario where the title was unable to set the user to Inactive before being interrupted.
				In both of these cases, your title should attempt to reconnect the user with the game and the other users to continue playing, or remove the user from the session.
			*/
			if (HrFailed(XblMultiplayerSessionLeave(SessionHandle), "[Session] Leaving the current game session")) /* leave the session as the current user */
				return false;
			std::shared_ptr< const XBLUserInfo > User = IdentityInterface->GetUserFromXuid(member.Xuid);
			if (!User) {
				UE_LOG_ONLINE(Warning, TEXT("[Session] Failed to find user with GamerTag='%s', XUID='%ud'"), *Gamertag, member.Xuid);
				return false;
			}

			auto status = XblMultiplayerWriteSessionAsync(
				User->XBLcontext
				, SessionHandle
				, XblMultiplayerSessionWriteMode::SynchronizedUpdate
				, AsyncTasks::CreateAsyncBlock([](XAsyncBlock* async) {}
				, IdentityInterface->GetDungeonsOnlineSubsystem()->GetQueueHandle())
			);
			if (HrFailed(status, "[Session] Writing session changes"))
				return false;

			UE_LOG_ONLINE(
				Warning
				, TEXT("[Session] Current player is detected in a lobby, but not in the game.\n This is a result of a force-disconnect.\n Leaving the session.\n GamerTag='%s', XUID='%ud'")
				, *Gamertag
				, member.Xuid
			);
			return true;
		}
	}
#endif //PLATFORM_XBOXONE
	return false;
}

void OnlineAsyncTaskDungeonsQueryLobbies::QuerySessions(const std::vector<XblMultiplayerActivityDetails>& sessions)
{
	FOnlineIdentityDungeons* IdentityInterface = Subsystem->GetDungeonsIdentityInterface();
	const auto user = IdentityInterface->GetUserFromLocalUser(SearchingPlayerNum);
	if (!user) {
		UE_LOG_ONLINE(Warning, TEXT("[QueryLobbies] The local player who QuerySessions doesn't exist any longer."));
		return;
	}

	for (const XblMultiplayerActivityDetails& session : sessions)
	{
		uint32_t MaxMembers = session.MaxMembersCount;
		uint32_t MembersCount = session.MembersCount;
		++PendingCalls;

		const auto hr = XblMultiplayerGetSessionAsync(
			user->XBLcontext,
			&session.SessionReference,
			AsyncTasks::CreateAsyncBlock([this, HandleId = FString(session.HandleId), IdentityInterface, MaxMembers, MembersCount](XAsyncBlock* asyncBlock) {
			--PendingCalls;
			check(PendingCalls >= 0 && "Query session pending calls out of sync");
			XblMultiplayerSessionHandle SessionHandle = nullptr;
			const auto hr = HrLog(XblMultiplayerGetSessionResult(asyncBlock, &SessionHandle), "[QueryLobbies] XblMultiplayerGetSessionResult");
			if (FAILED(hr)) {
				check(SessionHandle == nullptr);
				if (hr == E_XAL_UIREQUIRED) {
					UE_LOG_ONLINE(Warning, TEXT("[QueryLobbies] XblMultiplayerGetSessionResult fail indicates UI is required to solve an auth issue, logging out"));
					IdentityInterface->Logout(IdentityInterface->GetFirstLocalUserNumber());
				}
				return;
			}
			if (!SessionHandle) {
				//@AA This has been seen right after being kicked from the session as a client
				UE_LOG_ONLINE(Error, TEXT("[QueryLobbies] SessionHandle was null without XblMultiplayerGetSessionResult returning an error, this should not happen"));
				return;
			}
			scopedLambda::ScopedLambda s([SessionHandle] { XblMultiplayerSessionCloseHandle(SessionHandle); });

			const auto properties = sessionProperties::GetProperties(SessionHandle);
			if (!properties) {
				UE_LOG_ONLINE(Log, TEXT("[QueryLobbies] could not get the properties from a session handler, see previous error."));
				return;
			}

			if (properties->Version != sessionProperties::SessionProperties::CURRENT_VERSION) {
				UE_LOG_ONLINE(Log, TEXT("[QueryLobbies] Ignoring settings without version."));
				return;
			}

			if (!properties->IsSessionReadyToJoin) {
				UE_LOG_ONLINE(Log, TEXT("[QueryLobbies] Session is not ready to be joined"));
				return;
			}

			const auto IsSessionOwner = [OwningUserName = properties->OwningUserName](const FOnlineSessionSearchResult& searchResult) {
				return searchResult.Session.OwningUserName == OwningUserName.c_str();
			};

			if (SearchSettings->SearchResults.ContainsByPredicate(IsSessionOwner)) {
				return;
			}
			if (CheckForceDisconnectedMembers(IdentityInterface, SessionHandle)) {
				return; /* Ignore this session for now - it will be updated the next time. */
			}
			SearchSettings->SearchResults.Add(
				BuildSessionFromXBLSession(properties.GetValue(), SessionHandle, HandleId, MaxMembers, MaxMembers - MembersCount));
		}
		, IdentityInterface->GetDungeonsOnlineSubsystem()->GetQueueHandle()
		));

		if (HrFailed(hr, "[QueryLobbies] Get session when querying session")) {
			--PendingCalls;
		}
	}
}