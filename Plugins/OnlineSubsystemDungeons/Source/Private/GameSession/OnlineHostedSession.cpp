#include "PCHOnlineDungeonsSubsystem.h"

#include "OnlineHostedSession.h"
#include "SessionProperties.h"
#include "OnlineSubsystem.h"
#include "HrLog.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineSubsystemDungeons.h"
#include "OnlineFriendsInterface.h"

void FOnlineHostedSession::Tick(float DeltaTime)
{
	if (bIsCreatingSession)
	{
		if (TimeToCreateSession > 0)
		{
			TimeToCreateSession -= DeltaTime;
		}
		else
		{
			OnSessionCreatedFailed();
			OnSessionCreatedCallback(0, {});
		}
	}
}

TUniquePtr<IOnlineGameSession> FOnlineHostedSession::CreateSession(
	FOnlineIdentityDungeons* OnlineIdentity,
	int32 HostingPlayerNum,
	TSharedPtr<FNamedOnlineSession> Session,
	XTaskQueueHandle TaskQueueHandle,
	std::function<void(uint64_t uXUID, TOptional<XblGuid> correlationId)> OnSessionCreatedCallback,
	std::function<void()> OnServiceDisconnectedCallback,
	std::function<void(const FUniqueNetId& XUID, ESessionFailure::Type)> OnMemberLeftCallback
) {
	check(OnSessionCreatedCallback);
	check(OnServiceDisconnectedCallback);
	check(OnMemberLeftCallback);

	auto ptr = TUniquePtr<FOnlineHostedSession>(new FOnlineHostedSession(OnlineIdentity));

	ptr->HostingPlayerNum = HostingPlayerNum;
	ptr->HostedSession = Session;
	ptr->OnSessionCreatedCallback = OnSessionCreatedCallback;
	ptr->OnServiceDisconnectedCallback = OnServiceDisconnectedCallback;
	ptr->OnMemberLeftCallback = OnMemberLeftCallback;

	if (ptr->CreateXblSession(TaskQueueHandle)) {
		return std::move(ptr);
	}
	else {
		return {};
	}
}


void FOnlineHostedSession::HandleEvent(const XblMultiplayerEvent& mpEvent) {
	switch (mpEvent.EventType)
	{
	case XblMultiplayerEventType::UserAdded:
	{
		bIsCreatingSession = false;
		LocalUserAdded(mpEvent);
		break;
	}
	case XblMultiplayerEventType::MemberJoined:
	{
		GameSessionBase.MemberJoined(mpEvent);
		break;
	}
	case XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService:
	{
		if (OnServiceDisconnectedCallback) {
			OnServiceDisconnectedCallback();
		}
		break;
	}
	case XblMultiplayerEventType::MemberLeft:
	{
		MemberLeft(mpEvent);
		break;
	}
	}
}

bool FOnlineHostedSession::CreateXblSession(XTaskQueueHandle TaskQueueHandle) {
	const XblUserHandle user = GameSessionBase.GetOnlineIdentity()->GetXBLUserHandle(HostingPlayerNum);

	if (!user)
	{
		UE_LOG_ONLINE(Warning, TEXT("[Session] User not logged in"));
		return false;
	}

	UE_LOG_ONLINE(Log, TEXT("[Session] Creating new lobby session (HostingPlayerNum='%d', Name='%s')"), HostingPlayerNum, *HostedSession->SessionName.ToString());

	HrCheck(XblMultiplayerManagerInitialize(OnlineSubsystemConstants::DungeonsLobbySessionName.c_str(), TaskQueueHandle), "[Session] XblMultiplayerManagerInitialize");
	
	if (HrFailed(XblMultiplayerManagerLobbySessionAddLocalUser(user), "[Session] XblMultiplayerManagerLobbySessionAddLocalUser"))	{
		return false;
	}

	dungeonsSession::local::SetGUIDConnectionAddress(user);

	if (HostedSession->SessionSettings.bIsLANMatch) {
		HrCheck(XblMultiplayerManagerSetJoinability(XblMultiplayerJoinability::InviteOnly, nullptr), "[Session] XblMultiplayerManagerSetJoinability to invite only");
	}
	else {
		HrCheck(XblMultiplayerManagerSetJoinability(XblMultiplayerJoinability::JoinableByFriends, nullptr), "[Session] XblMultiplayerManagerSetJoinability to joinable by friends");
	}

	sessionProperties::SetProperties(HostedSession.Get());
	sessionProperties::SetIsReady(false);

	bIsCreatingSession = true;
	TimeToCreateSession = OnlineSubsystemConstants::MAX_TIME_TO_CREATE_XBL_SESSION_SECONDS;

	return true;
}

void FOnlineHostedSession::LocalUserAdded(const XblMultiplayerEvent& mpEvent) {
	uint64_t uXUID = 0;
	HrLog(XblMultiplayerEventArgsXuid(mpEvent.EventArgsHandle, &uXUID), "[Session] XblMultiplayerEventArgsXuid (UserAdded)");

	TOptional<XblGuid> correlationId;
	if (HrSuccess(mpEvent.Result, "[Session] Add local user event")) {
		XblGuid correlationIdRaw;
		if (HrSuccess(XblMultiplayerManagerLobbySessionCorrelationId(&correlationIdRaw), "[Session] XblMultiplayerManagerLobbySessionCorrelationId")) {
			correlationId = correlationIdRaw;
		}
	}

	if (auto User = GameSessionBase.GetOnlineIdentity()->GetUserFromXuid(uXUID))
	{
		dungeonsSession::local::SetPlatformProperty(User->XBLUser);
	}
	else
	{
		UE_LOG_ONLINE(Log, TEXT("[Session] Failed to get user when setting platform property (XUID=%d)"), uXUID);
		uXUID = 0; //This is currently how we signal to the Session interface that we failed to get User
	}

	if (OnSessionCreatedCallback) {
		OnSessionCreatedCallback(uXUID, correlationId);
	}
}

void FOnlineHostedSession::MemberLeft(const XblMultiplayerEvent& mpEvent)
{
	size_t MemberCount = 0;
	std::vector<XblMultiplayerManagerMember> Members;

	GameSessionBase.MemberLeft(mpEvent);

	if (HrFailed(XblMultiplayerEventArgsMembersCount(mpEvent.EventArgsHandle, &MemberCount), "[Session] XblMultiplayerEventArgsMembersCount (Memberleft)"))
	{
		return;
	}

	Members.resize(MemberCount);

	HrLog(XblMultiplayerEventArgsMembers(mpEvent.EventArgsHandle, MemberCount, Members.data()), "[Session] XblMultiplayerEventArgsXuid (MemberLeft)");

	for (size_t i = 0; i < MemberCount; i++)
	{
		uint64_t uXUID = Members[i].Xuid;
		FUniqueNetIdDungeons NetId(uXUID);
		if (*HostedSession->OwningUserId.Get() != NetId)
		{
			OnMemberLeftCallback(NetId, ESessionFailure::ServiceConnectionLost);
		}
	}
	GameSessionBase.GetOnlineSubsystem()->GetFriendsInterface()->TriggerOnFriendsChangeDelegates(0);

}

void FOnlineHostedSession::OnSessionCreatedFailed()
{
	bIsCreatingSession = false;
	HrCheck(XblMultiplayerManagerInitialize(OnlineSubsystemConstants::DungeonsLobbySessionName.c_str(), GameSessionBase.GetOnlineIdentity()->GetDungeonsOnlineSubsystem()->GetQueueHandle()), "[Session] XblMultiplayerManagerInitialize");
}

