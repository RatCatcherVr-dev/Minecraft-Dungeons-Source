#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineAsyncTaskDungeonsFindLobbies.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineSessionInterfaceDungeons.h"
#include "OnlineIdentityDungeons.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineAsyncTaskDungeonsQueryLobbies.h"
#include "OnlineAsyncTaskDungeonsTriggers.h"

#include "ScopeLock.h"
#include "utils.h"

#include "cpprestinclude/cpprest/json.h"
#include "Private/utils.h"
#include "HrLog.h"

#if !PLATFORM_SWITCH && !PLATFORM_PS4
#include "AllowWindowsPlatformAtomics.h"
#include "AllowWindowsPlatformTypes.h"
#endif

#if !PLATFORM_XBOXONE
#include "xal_platform.h"
#include "Xal/xal.h"
#include "xal_user.h"
#endif

#if !PLATFORM_SWITCH && !PLATFORM_PS4
#include "HideWindowsPlatformTypes.h"
#include "HideWindowsPlatformAtomics.h"
#endif

class FUniqueNetIdDungeons;
class FOnlineSessionInfoDungeons;

OnlineAsyncTaskDungeonsFindLobbies::OnlineAsyncTaskDungeonsFindLobbies(FOnlineSubsystemDungeons* InSubsystem
	, const TSharedPtr<FOnlineSessionSearch> InSearchSettings
	, int32 InSearchingPlayerNum)
	: Subsystem(InSubsystem), SearchSettings(InSearchSettings), SearchingPlayerNum(InSearchingPlayerNum)
{
	bIsComplete = true;
	
	FOnlineIdentityDungeons* IdentityInterface = Subsystem->GetDungeonsIdentityInterface();
	if (!IdentityInterface)
	{
		UE_LOG_ONLINE(Error, TEXT("[FindLobbies] Failed to get identity interface (nullptr)"));
		Fail();
		return;
	}

	if (IdentityInterface->GetLoginStatus(SearchingPlayerNum) == ELoginStatus::NotLoggedIn)
	{
		UE_LOG_ONLINE(Error, TEXT("[FindLobbies] Searching player is not logged in"));
		Fail();
		return;
	}

	const auto user = IdentityInterface->GetUserFromLocalUser(SearchingPlayerNum);
	if (!user)
	{
		UE_LOG_ONLINE(Error, TEXT("[FindLobbies] User not found"));
		Fail();
		return;
	}

	const HRESULT hr = XblMultiplayerGetActivitiesForSocialGroupAsync(
		user->XBLcontext,
		OnlineSubsystemConstants::MULTIPLAYERSCID,
		user->UserId,
		"People",
		AsyncTasks::CreateAsyncBlock([Subsystem = Subsystem, SearchSettings = SearchSettings, SearchingPlayerNum = SearchingPlayerNum] (XAsyncBlock* asyncBlock)
	{
		size_t Count = 0;
		IOnlineSessionPtr SessionInt = Subsystem->GetSessionInterface();
		const auto hr = HrLog(XblMultiplayerGetActivitiesForSocialGroupResultCount(asyncBlock, &Count), "[FindLobbies] Getting count for sessions");
		if (FAILED(hr))
		{
			SearchSettings->SearchState = EOnlineAsyncTaskState::Done;
			Subsystem->QueueAsyncTask([SessionInt]() {
				SessionInt->TriggerOnFindSessionsCompleteDelegates(false);
			});
			if (hr == E_XAL_UIREQUIRED) {
				UE_LOG_ONLINE(Warning, TEXT("[FindLobbies] Failed finding sessions indicates UI is required to solve an auth issue, logging out"));
				Subsystem->GetIdentityInterface()->Logout(SearchingPlayerNum);
			}
			return;
		}

		if (Count == 0)
		{
			SearchSettings->SearchState = EOnlineAsyncTaskState::Done;
			Subsystem->QueueAsyncTask([SessionInt]() {
				SessionInt->TriggerOnFindSessionsCompleteDelegates(true);
			});
			return;
		}

		std::vector<XblMultiplayerActivityDetails> payload(Count);
		if (HrFailed(XblMultiplayerGetActivitiesForSocialGroupResult(asyncBlock, Count, payload.data()), "[FindLobbies] Getting payload"))
		{
			SearchSettings->SearchState = EOnlineAsyncTaskState::Done;
			return;
		}

		auto QueryLobby = MakeUnique<OnlineAsyncTaskDungeonsQueryLobbies>(Subsystem, SearchSettings, SearchingPlayerNum, payload);
		Subsystem->QueueAsyncTask(std::move(QueryLobby));
	}, 
	Subsystem->GetQueueHandle()));

	if (HrFailed(hr, "[FindLobbies] XblMultiplayerGetActivitiesForSocialGroupAsync"))
	{
		Fail();
	}
	else 
	{
		SearchSettings->SearchState = EOnlineAsyncTaskState::InProgress;
		bWasSuccessful = true;
	}
}

void OnlineAsyncTaskDungeonsFindLobbies::Tick(){}
void OnlineAsyncTaskDungeonsFindLobbies::Finalize(){}
void OnlineAsyncTaskDungeonsFindLobbies::TriggerDelegates(){}

void OnlineAsyncTaskDungeonsFindLobbies::Fail() {
	bWasSuccessful = false;
	SearchSettings->SearchState = EOnlineAsyncTaskState::Done;
}
