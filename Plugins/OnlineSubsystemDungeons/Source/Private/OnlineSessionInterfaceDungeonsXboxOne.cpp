#if PLATFORM_XBOXONE
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
#include "AllowWindowsPlatformTypes.h"
#include "utils.h"
#include "HideWindowsPlatformTypes.h"

#pragma warning(disable:4668)
#include "AllowWindowsPlatformAtomics.h"
#include "AllowWindowsPlatformTypes.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include "HideWindowsPlatformTypes.h"
#include "HideWindowsPlatformAtomics.h"
#pragma warning(default:4668)

#include "ScopeLock.h"
#include "AsyncTasks/OnlineAsyncTaskDungeonsFindLobbies.h"
#include "AsyncTasks/OnlineAsyncTaskDungeonsTriggers.h"
#include "SharedPointer.h"
#include "Engine/EngineBaseTypes.h"
#include "SessionProperties.h"
#include "GameSession/OnlineHostedSession.h"
#include "GameSession/OnlineClientSession.h"
#include "HrLog.h"
#include "IPAddress.h"

#include "XboxOne/XboxOneMisc.h"
#include "XboxOne/XboxOneApplication.h"
#include "XboxOne/XboxOneMemory.h"
#include <collection.h>
#include <Runtime/Json/Public/Json.h>

#include "interfaces/OnlineIdentityInterface.h"

using namespace Platform::Collections;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Xbox::System;
using namespace Microsoft::Xbox::Services;
using namespace Microsoft::Xbox::Services::Multiplayer;
using Microsoft::Xbox::Services::XboxLiveContext;

#include "OnlineSessionInterfaceDungeonsXboxOne.h"
#include "P2P/IpAddressDungeonsWebRTC.h"

FOnlineSessionDungeonsPtr dungeonsSession::platform::Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem)
{
	return MakeShared<FOnlineSessionDungeonsXboxOne, ESPMode::ThreadSafe>(InDungeonsSubsystem);
}

namespace
{
	/** The maximum number of Live sessions to return when searching for orphaned sessions. */
	const int32 MAX_ORPHANED_SESSIONS_RESULTS = 100;
	const int32 MAX_RETRIES = 20;

	/** Gets the current time as a DateTime object */
	DateTime GetCurrentTime()
	{
		ULARGE_INTEGER uInt;
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		uInt.LowPart = ft.dwLowDateTime;
		uInt.HighPart = ft.dwHighDateTime;

		Windows::Foundation::DateTime time;
		time.UniversalTime = uInt.QuadPart;
		return time;
	}

	const TCHAR* GetSessionMemberStatusString(MultiplayerSessionMemberStatus Status)
	{

		switch (Status)
		{
		case MultiplayerSessionMemberStatus::Active:
			return TEXT("Active");
		case MultiplayerSessionMemberStatus::Inactive:
			return TEXT("Inactive");
		case MultiplayerSessionMemberStatus::Ready:
			return TEXT("Ready");
		case MultiplayerSessionMemberStatus::Reserved:
			return TEXT("Reserved");
		}

		return TEXT("Unknown");
	}

	uint64_t GetXuid(Platform::String^ UserXuid)
	{
		std::wstring fooW(UserXuid->Begin());
		std::string fooA(fooW.begin(), fooW.end());
		return std::stoull(fooA);
	}

	Windows::Xbox::System::User^ GetJoiningUser(Platform::String^ UserXuid)
	{
		Windows::Xbox::System::User^ JoiningUser = nullptr;
		for (Windows::Xbox::System::User^ user : Windows::Xbox::System::User::Users)
		{
			if (user->XboxUserId == UserXuid)
			{
				JoiningUser = user;
			}
		}
		return JoiningUser;
	}
}

FOnlineSessionDungeonsXboxOne::FOnlineSessionDungeonsXboxOne(FOnlineSubsystemDungeons* InSubsystem)
	: FOnlineSessionDungeons(InSubsystem)
{
	ActivatedToken = CoreApplication::GetCurrentView()->Activated += ref new TypedEventHandler< CoreApplicationView^, IActivatedEventArgs^ >(
		[this](CoreApplicationView^, IActivatedEventArgs^ EventArgs)
	{
		OnActivated(EventArgs);
	});

	CheckInviteFromOutsideGame();

}

void FOnlineSessionDungeonsXboxOne::CheckInviteFromOutsideGame()
{
	// If the title was protocol activated before OSS was initialized (likely at launch), the startup code saved the
	// activation URI.
	const FString& ActivationUriString = FXboxOneMisc::GetProtocolActivationUri();

	if (!ActivationUriString.IsEmpty())
	{
		Windows::Foundation::Uri^ ActivationUri = ref new Windows::Foundation::Uri(ref new Platform::String(*ActivationUriString));
		// See if this activation was in response to a session invite
		if (ActivationUri->Host == "inviteHandleAccept")
		{
			SaveInviteFromActivation(ActivationUri);

		}
		else if (ActivationUri->Host == "activityHandleJoin")
		{
			SaveJoinFromActivation(ActivationUri);
		}
		FXboxOneMisc::SetProtocolActivationUri(FString());
	}
}

FOnlineSessionDungeonsXboxOne::~FOnlineSessionDungeonsXboxOne()
{
}

void FOnlineSessionDungeonsXboxOne::PlatformTick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Session_Interface);

	TickPendingSession(DeltaTime);
}

void FOnlineSessionDungeonsXboxOne::OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs^ EventArgs)
{
	if (EventArgs->Kind == Windows::ApplicationModel::Activation::ActivationKind::Protocol)
	{
		ProtocolActivatedEventArgs^ ProtocolArgs = static_cast<ProtocolActivatedEventArgs^>(EventArgs);
		Windows::Foundation::Uri^ ActivationUri = ref new Windows::Foundation::Uri(ProtocolArgs->Uri->RawUri);

		UE_LOG_ONLINE(Log, TEXT("[Session]----- Got activation URI: %ls"), ActivationUri->AbsoluteUri->Data());

		// See if this activation was in response to a session invite or gamercard join
		
		if (ActivationUri->Host == "inviteHandleAccept")
		{
			SaveInviteFromActivation(ActivationUri);
		}
		else if (ActivationUri->Host == "activityHandleJoin")
		{
			SaveJoinFromActivation(ActivationUri);
		}
	}
}


void FOnlineSessionDungeonsXboxOne::TickPendingSession(float DeltaTime)
{
	if (Subsystem->GetDungeonsIdentityInterface()->GetFirstLocalUserNumber() == -1)
	{
		return;
	}

	if (!PendingSession.bHaveSessionInfo || PendingSession.bPrivilegeCheckInProgress)
	{
		return;
	}

	if (PendingSession.AcceptingUser == nullptr)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] FOnlineSessionDungeonsXboxOne::TickPendingInvites: bHaveSessionInfo is true but AcceptingUser is null."));
		PendingSession.bHaveSessionInfo = false;
		return;
	}

	if (PendingSession.SessionHandle == nullptr)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] FOnlineSessionDungeonsXboxOne::TickPendingInvites: bHaveSessionInfo is true but SessionHandle is null."));
		PendingSession.bHaveSessionInfo = false;
		return;
	}

	uint64_t Xuid = GetXuid(PendingSession.AcceptingUser->XboxUserId);

	FPlatformUserId AcceptingUserIndex = 0;
	if (std::shared_ptr<const XBLUserInfo> userInfo = GetIdentityInterface()->GetUserFromXuid(Xuid))
	{
		AcceptingUserIndex = userInfo->LocalUserNum;
		if (AcceptingUserIndex < 0)
		{
			PendingSession.bHaveSessionInfo = false;
			return;
		}
	}
	else if (GetIdentityInterface()->GetLocalUsersNum() > 0)
	{
		PendingSession.bHaveSessionInfo = false;
		return;
	}

	FUniqueNetIdDungeonsRef UniqueNetId = FUniqueNetIdDungeons::Create(Xuid);
	const FString SessionInviteHandle(PendingSession.SessionHandle->Data());

	const auto Identity = Subsystem->GetDungeonsIdentityInterface();

	PendingSession.bPrivilegeCheckInProgress = true;

	Identity->GetUserPrivilege(*UniqueNetId, EUserPrivileges::CanPlayOnline, IOnlineIdentity::FOnGetUserPrivilegeCompleteDelegate::CreateLambda([&,Identity, UniqueNetId, AcceptingUserIndex](const FUniqueNetId& userId, EUserPrivileges::Type privilege, uint32 privilegeResult) {
		if (const auto User = Identity->GetUserFromLocalUser(Identity->GetFirstLocalUserNumber())) {
			PendingSession.bPrivilegeCheckInProgress = false;
			FString HandleId(PendingSession.SessionHandle->Data());

			XblMultiplayerGetSessionByHandleAsync(User->XBLcontext, TCHAR_TO_ANSI(*HandleId), AsyncTasks::CreateAsyncBlock([&,UniqueNetId, User, HandleId, AcceptingUserIndex](XAsyncBlock* async) {
				XblMultiplayerSessionHandle Handle = nullptr;
				HRESULT Hr = XblMultiplayerGetSessionByHandleResult(async, &Handle);
				if (Hr == HTTP_E_STATUS_FORBIDDEN)
				{
					TriggerOnSessionFailureDelegates(FUniqueNetIdString("JoinPermissionError"), ESessionFailure::ServiceConnectionLost);
					HrLog(Hr, "[Session] XblMultiplayerGetSessionByHandleResult Failed because user does not have permission");
					return;
				}
				if (!SUCCEEDED(Hr))
				{
					TriggerOnSessionUserInviteAcceptedDelegates(false, User->LocalUserNum, UniqueNetId, FOnlineSessionSearchResult());
					HrLog(Hr, "[Session] XblMultiplayerGetSessionByHandleResult Failed for some reason");
					return;
				}
				// D11.PC  In case the handle was not retrieve and was not a forbidden issue
				if (!Handle)
				{
					TriggerOnSessionUserInviteAcceptedDelegates(false, User->LocalUserNum, UniqueNetId, FOnlineSessionSearchResult());
					UE_LOG_ONLINE(Log, TEXT("[Session]----- Failed because Handle = nullptr"));
					return;
				}

				scopedLambda::ScopedLambda s([Handle] { XblMultiplayerSessionCloseHandle(Handle); });

				size_t MemberCount = 0;
				const XblMultiplayerSessionMember* Members = nullptr;

				if (HrFailed(XblMultiplayerSessionMembers(Handle, &Members, &MemberCount), "[Session] Getting session members"))
				{
					return;
				}

				for (size_t i = 0; i < MemberCount; i++) {
					const XblMultiplayerSessionMember& member = Members[i];
					if (UniqueNetId->ToString() == std::to_string(member.Xuid).c_str()) {
						TriggerOnSessionUserInviteAcceptedDelegates(true, User->LocalUserNum, UniqueNetId, FOnlineSessionSearchResult());
						// Already present in session that im trying to join, cancelling
						return;
					}
				}
				const auto Properties = sessionProperties::GetProperties(Handle);
				auto Session = BuildSessionFromXBLSession(Properties.GetValue(), Handle, HandleId, 4 - MemberCount, 4 - MemberCount);

				TriggerOnSessionUserInviteAcceptedDelegates(true, AcceptingUserIndex, UniqueNetId, Session);
			}, Subsystem->GetQueueHandle()));
		}
		else
		{
			PendingSession.bHaveSessionInfo = false;
			return;
		}

		PendingSession.bHaveSessionInfo = false;
		PendingSession.bPrivilegeCheckInProgress = false;
		PendingSession.AcceptingUser = nullptr;
		PendingSession.SessionHandle = nullptr;
	}));
}

bool FOnlineSessionDungeonsXboxOne::SaveInviteFromActivation(Windows::Foundation::Uri^ ActivationUri)
{
	Platform::String^ SessionHandle = ActivationUri->QueryParsed->GetFirstValueByName("handle");
	Platform::String^ UserXuid = ActivationUri->QueryParsed->GetFirstValueByName("invitedXuid");
	Windows::Xbox::System::User^ JoiningUser = GetJoiningUser(UserXuid);

	if (JoiningUser && SessionHandle)
	{
		SaveActivationSession(JoiningUser, SessionHandle);
		return true;
	}
	return true;
}

bool FOnlineSessionDungeonsXboxOne::SaveJoinFromActivation(Windows::Foundation::Uri^ ActivationUri)
{
	Platform::String^ SessionHandle = ActivationUri->QueryParsed->GetFirstValueByName("handle");
	Platform::String^ UserXuid = ActivationUri->QueryParsed->GetFirstValueByName("joinerXuid");
	Windows::Xbox::System::User^ JoiningUser = GetJoiningUser(UserXuid);

	if (JoiningUser && SessionHandle) 
	{
		SaveActivationSession(JoiningUser, SessionHandle);
		return true;
	}
	return false;
}

void FOnlineSessionDungeonsXboxOne::SaveActivationSession(Windows::Xbox::System::User^ AcceptingUser, Platform::String^ SessionHandle)
{
	// Set the invite data on the game thread since that's where it will be consumed
	Subsystem->ExecuteNextTick([this, AcceptingUser, SessionHandle]
	{
		PendingSession.AcceptingUser = AcceptingUser;
		PendingSession.SessionHandle = SessionHandle;
		PendingSession.bHaveSessionInfo = true;
	});
}
#endif