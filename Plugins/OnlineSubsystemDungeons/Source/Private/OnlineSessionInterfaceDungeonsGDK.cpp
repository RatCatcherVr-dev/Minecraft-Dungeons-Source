#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineSessionInterfaceDungeonsGDK.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineSubsystemTypes.h"
#include "OnlineSessionSettings.h"
#include "SessionProperties.h"

#include "SharedPointer.h"
#include "GDKDungeons.h"
#include "HrLog.h"
#include "utils.h"

namespace {
	TOptional<FString> GetUriQueryStringParameter(std::string uri, std::string name)
	{
		auto namePos = uri.find(name + "=");
		if (namePos == std::string::npos || namePos + name.size() > uri.size()) {
			return TOptional<FString>{};
		}

		auto startOfParam = namePos + name.size() + 1;
		auto endOfParam = uri.find('&', namePos);

		if (endOfParam == std::string::npos) {
			return FString(uri.substr(startOfParam).data());
		} else {
			return FString(uri.substr(startOfParam, endOfParam - startOfParam).data());
		}
	}
}

FOnlineSessionDungeonsPtr dungeonsSession::platform::Create(class FOnlineSubsystemDungeons* InDungeonsSubsystem)
{
	return MakeShared<FOnlineSessionDungeonsGDK, ESPMode::ThreadSafe>(InDungeonsSubsystem);
}

FOnlineSessionDungeonsGDK::FOnlineSessionDungeonsGDK(FOnlineSubsystemDungeons* InSubsystem)
	: FOnlineSessionDungeons(InSubsystem)
{
	// do GDK invite wireup
	auto& gdk = FGDKDungeonsInterface::Get();
	HrLog(gdk.InitializeGDK(), TEXT("[Session] InitializeGDK"));
	HrLog(gdk.RegisterForInvites(this, [](void* context, const char* inviteUri) {
			auto thisptr = static_cast<FOnlineSessionDungeonsGDK*>(context);
			thisptr->OnInviteReceived(inviteUri);
		}, &registrationToken),
		TEXT("[Session] Registered for gamebar invites"));
}

FOnlineSessionDungeonsGDK::~FOnlineSessionDungeonsGDK()
{
	// unwire invite
	auto& gdk = FGDKDungeonsInterface::Get();
	bool result = gdk.UnregisterForInvites(registrationToken, true);
	UE_LOG_ONLINE(Log, TEXT("[Session] Unregistered for gamebar invites result %d"), result);
	result = gdk.UninitializeGDK();
	UE_LOG_ONLINE(Log, TEXT("[Session] Unregister GDK result %d"), result);
}

void FOnlineSessionDungeonsGDK::PlatformTick(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_Session_Interface);
	TickPendingSession(DeltaTime);
}

void FOnlineSessionDungeonsGDK::OnInviteReceived(const char* inviteUri)
{
	UE_LOG_ONLINE(Log, TEXT("[Session] Got invite URI: %S"), inviteUri);
	std::string inviteString(inviteUri);

	// Invite URIs look like one of these two:
	// ms-xbl-multiplayer://inviteHandleAccept?handle=3bbd5509-1db7-4ce8-95e7-b5f60a36ab3d&invitedXuid=2814639838850350&senderXuid=2814660898764922&context=Join%2520my%2520game%2521
	// ms-xbl-multiplayer://activityHandleJoin?joinerXuid=2814639838850350&handle=485ba369-1268-43a6-bf5d-9761d68373ed&joineeXuid=2814660898764922

	bool IsInvite = inviteString.find("inviteHandleAccept") != std::string::npos;

	PendingSession.SessionHandle = GetUriQueryStringParameter(inviteString, "handle");
	if (!PendingSession.SessionHandle.IsSet()) {
		UE_LOG_ONLINE(Error, TEXT("[Session] Could not find invite handle in uri"));
		return;
	}

	TOptional<FString> joinerXuid;
	if (IsInvite) {
		joinerXuid = GetUriQueryStringParameter(inviteString, "invitedXuid");
	} else {
		joinerXuid = GetUriQueryStringParameter(inviteString, "joinerXuid");
	}
	if (!joinerXuid.IsSet()) {
		UE_LOG_ONLINE(Error, TEXT("[Session] Could not find invited/joiner xuid"));
		return;
	}

	PendingSession.joiningUserXuid = FCString::Strtoui64(*joinerXuid.GetValue(), nullptr, 10);
	if (PendingSession.joiningUserXuid == 0) {
		UE_LOG_ONLINE(Error, TEXT("[Session] Unable to parse invited/joiner xuid {%s}"), *joinerXuid.GetValue());
		return;
	}

	PendingSession.bHaveSessionInfo = true;
}

void FOnlineSessionDungeonsGDK::TickPendingSession(float DeltaTime)
{
	//Do not process invite/join before we are signed in
	if (Subsystem->GetDungeonsIdentityInterface()->GetFirstLocalUserNumber() == -1)
	{
		return;
	}

	scopedLambda::ScopedLambda lambda([&PendingSession = PendingSession]() { PendingSession.Clear(); });
	if (!PendingSession.bHaveSessionInfo)
	{
		return;
	}

	if (PendingSession.joiningUserXuid == 0)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] FOnlineSessionDungeonsGDK::TickPendingInvites: bHaveSessionInfo is true but joiningUserXuid is 0."));
		return;
	}

	if (!PendingSession.SessionHandle.IsSet())
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] FOnlineSessionDungeonsGDK::TickPendingInvites: bHaveSessionInfo is true but SessionHandle is empty."));
		return;
	}

	FUniqueNetIdDungeonsRef UniqueNetId = FUniqueNetIdDungeons::Create(PendingSession.joiningUserXuid);

	//Make sure that the joining user is the same user that accepted the invite.
	const auto Identity = Subsystem->GetDungeonsIdentityInterface();
	if (const auto User = Identity->GetUserFromXuid(PendingSession.joiningUserXuid))
	{
		FString HandleId = PendingSession.SessionHandle.GetValue();
		UE_LOG_ONLINE(Log, TEXT("[Session] Joining session %s with user %llu"), *HandleId, PendingSession.joiningUserXuid);

		const auto hr = XblMultiplayerGetSessionByHandleAsync(
			User->XBLcontext,
			TCHAR_TO_ANSI(*HandleId),
			AsyncTasks::CreateAsyncBlock([SessionInterface = this, HandleId, LocalUserNum = User->LocalUserNum, PendingSession = PendingSession, UniqueNetId](XAsyncBlock* async)
		{
			XblMultiplayerSessionHandle Handle = nullptr;
			HRESULT Hr = HrLog(XblMultiplayerGetSessionByHandleResult(async, &Handle), "[Session] XblMultiplayerGetSessionByHandleResult");
			if (Hr == HTTP_E_STATUS_FORBIDDEN)
			{
				SessionInterface->TriggerOnSessionUserInviteAcceptedDelegates(false, LocalUserNum, UniqueNetId, FOnlineSessionSearchResult());
				return;
			}
			if (!SUCCEEDED(Hr)) {
				SessionInterface->TriggerOnSessionUserInviteAcceptedDelegates(false, LocalUserNum, UniqueNetId, FOnlineSessionSearchResult());
				HrLog(Hr, "[Session] XblMultiplayerGetSessionByHandleResult Failed for some reason");
				return;
			}
			// D11.PC  In case the handle was not retrieve and was not a forbidden issue
			if (!Handle) {
				SessionInterface->TriggerOnSessionUserInviteAcceptedDelegates(false, LocalUserNum, UniqueNetId, FOnlineSessionSearchResult());
				UE_LOG_ONLINE(Log, TEXT("[Session]----- Failed because Handle = nullptr"));
				return;
			}

			scopedLambda::ScopedLambda s([Handle] { XblMultiplayerSessionCloseHandle(Handle); });

			size_t MemberCount = 0;
			const XblMultiplayerSessionMember* Members = nullptr;

			if (HrFailed(XblMultiplayerSessionMembers(Handle, &Members, &MemberCount), "[Session] XblMultiplayerSessionMembers"))
			{
				return;
			}

			const auto Properties = sessionProperties::GetProperties(Handle);
			auto Session = BuildSessionFromXBLSession(Properties.GetValue(), Handle, HandleId, 4 - MemberCount, 4 - MemberCount);

			SessionInterface->TriggerOnSessionUserInviteAcceptedDelegates(true, LocalUserNum, UniqueNetId, Session);
		}, Subsystem->GetQueueHandle()));

		if (FAILED(hr)) {
			UE_LOG_ONLINE(Error, TEXT("[Session] HResult %s when looking up session with xuid %llu and sessionHandle %s"), *ConvertHRtoString(hr), PendingSession.joiningUserXuid, *PendingSession.SessionHandle.GetValue());
		}
	}
}

#endif