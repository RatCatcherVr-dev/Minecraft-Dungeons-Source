#include "PCHOnlineDungeonsSubsystem.h"

#include "OnlineClientSession.h"
#include "HrLog.h"
#include "OnlineSubsystem.h"
#include "utils.h"
#include "OnlineSubsystemDungeonsTypes.h"
#include "OnlineSessionInterfaceDungeons.h"

TUniquePtr<IOnlineGameSession> FOnlineClientSession::JoinSession(
	FOnlineIdentityDungeons* OnlineIdentity,
	XblUserHandle UserHandle,
	const FString& HandleId,
	XTaskQueueHandle TaskQueueHandle,
	uint64_t HostXUID,
	std::function<void(EJoinResult)> OnJoinedCallback,
	std::function<void()> OnHostChangedCallback,
	std::function<void()> OnServiceDisconnectedCallback
)
{
	check(OnJoinedCallback);
	check(OnHostChangedCallback);
	check(OnServiceDisconnectedCallback);

	HrCheck(XblMultiplayerManagerInitialize(OnlineSubsystemConstants::DungeonsLobbySessionName.c_str(), TaskQueueHandle), "[Session] Initialize multiplayer manager");

	if (HrSuccess(XblMultiplayerManagerJoinLobby(TCHAR_TO_ANSI(*HandleId), UserHandle), "[Session] Joining lobby")) {
		auto ptr = TUniquePtr<FOnlineClientSession>(new FOnlineClientSession(OnlineIdentity));
		ptr->OnJoinedCallback = OnJoinedCallback;
		ptr->OnHostChangedCallback = OnHostChangedCallback;
		ptr->OnServiceDisconnectedCallback = OnServiceDisconnectedCallback;
		ptr->HostXUID = HostXUID;
		ptr->bIsJoiningSession = true;
		ptr->TimeToJoinSession = OnlineSubsystemConstants::MAX_TIME_TO_JOIN_XBL_SESSION_SECONDS;

		dungeonsSession::local::SetGUIDConnectionAddress(UserHandle);

		return std::move(ptr);
	}
	else {
		return {};
	}
}

void FOnlineClientSession::HandleEvent(const XblMultiplayerEvent& mpEvent) {
	switch (mpEvent.EventType)
	{
	case XblMultiplayerEventType::ClientDisconnectedFromMultiplayerService:
	{
		CallCallbackOnce([this] { OnServiceDisconnectedCallback(); });
		break;
	}
	case XblMultiplayerEventType::JoinLobbyCompleted:
	{
		HandleJoin(mpEvent);
		break;
	}
	case XblMultiplayerEventType::MemberJoined:
	{
		GameSessionBase.MemberJoined(mpEvent);
		break;
	}
	case XblMultiplayerEventType::HostChanged:
	{
		CallCallbackOnce([this] { OnHostChangedCallback(); });
		break;
	}
	case XblMultiplayerEventType::MemberLeft:
	{
		GameSessionBase.MemberLeft(mpEvent);
		if (!dungeonsSession::local::IsUserInLobby(HostXUID))
		{
			CallCallbackOnce([this] { OnHostChangedCallback(); });
		}
		break;
	}
	}
}

void FOnlineClientSession::Tick(float DeltaTime)
{
	if (bIsJoiningSession)
	{
		if (TimeToJoinSession > 0)
		{
			TimeToJoinSession -= DeltaTime;
		}
		else
		{
			bIsJoiningSession = false;

			CallCallbackOnce([this] { OnJoinedCallback(EJoinResult::GenericFail); });
		}
	}
}

void FOnlineClientSession::HandleJoin(const XblMultiplayerEvent& mpEvent) {
	if (HrSuccess(mpEvent.Result, "[Session] JoinLobbyCompleted event")) {
		uint64_t JoiningXUID = 0;
		HrCheck(XblMultiplayerEventArgsXuid(mpEvent.EventArgsHandle, &JoiningXUID), "[Session] Extracting the xuid from the multiplayer join event");

		if (!dungeonsSession::local::IsUserInLobby(HostXUID)){
			CallCallbackOnce([this] { OnJoinedCallback(EJoinResult::NotFound); });
		}
		else {
			if (const auto User = GameSessionBase.GetOnlineIdentity()->GetUserFromXuid(JoiningXUID))
			{
				dungeonsSession::local::SetPlatformProperty(User->XBLUser);
				for (const auto& member : dungeonsSession::global::getLobbySessionMembers())
				{
					GameSessionBase.GetOnlineSession()->RegisterPlayer(OnlineSubsystemConstants::DungeonsGameSessionName, FUniqueNetIdDungeons(member.Xuid), false);
				}

				CallCallbackOnce([this] { OnJoinedCallback(EJoinResult::Success); });
			}
			else
			{
				CallCallbackOnce([this] { OnJoinedCallback(EJoinResult::GenericFail); });
			}
		}
	}
	else if (mpEvent.Result == 404) {
		CallCallbackOnce([this] { OnJoinedCallback(EJoinResult::NotFound); });
	}
	else if (mpEvent.Result == HTTP_E_STATUS_BAD_REQUEST) {
		// Special case for XSAPI, see ADO ticket #307914
		// A bad request means either a malformed request to the XSAPI backend or that the session will become
		// invalid if the request would be fulfilled. In this case it would become invalid if number
		// of players would become greater than the maximum. We can therefore assume that the session
		// is full if we get the bad request result.
		CallCallbackOnce([this] { OnJoinedCallback(EJoinResult::SessionIsFull); });
	}
	else {
		CallCallbackOnce([this] { OnJoinedCallback(EJoinResult::GenericFail); });
	}
}

void FOnlineClientSession::CallCallbackOnce(std::function<void()> callback)
{
	bIsJoiningSession = false;

	if (!hasCallbackBeenCalled) {
		hasCallbackBeenCalled = true;
		callback();
	}
}

