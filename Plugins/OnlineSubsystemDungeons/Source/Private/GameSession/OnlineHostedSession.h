#pragma once

#include "utils.h"
#include "OnlineGameSession.h"
#include "OnlineGameSessionBase.h"
#include "OnlineSessionInterface.h"
#include <OnlineSessionSettings.h>

class FOnlineHostedSession : public IOnlineGameSession {
public:
	static TUniquePtr<IOnlineGameSession> CreateSession(
		FOnlineIdentityDungeons* OnlineIdentity,
		int32 HostingPlayerNum,
		TSharedPtr<FNamedOnlineSession> Session,
		XTaskQueueHandle TaskQueueHandle,
		std::function<void(uint64_t uXUID, TOptional<XblGuid> correlationId)> OnSessionCreatedCallback,
		std::function<void()> OnServiceDisconnectedCallback,
		std::function<void(const FUniqueNetId& XUID, ESessionFailure::Type)> OnMemberLeftCallback
	);

	void HandleEvent(const XblMultiplayerEvent&) override;

	virtual void Tick(float DeltaTime) override;

private:
	explicit FOnlineHostedSession(FOnlineIdentityDungeons* OnlineIdentity)
		: GameSessionBase(OnlineIdentity)
	{}

	bool CreateXblSession(XTaskQueueHandle);
	void LocalUserAdded(const XblMultiplayerEvent&);
	void MemberLeft(const XblMultiplayerEvent&);

	void OnSessionCreatedFailed();
	bool bIsCreatingSession = false;
	float TimeToCreateSession = 0;

	std::function<void(uint64_t uXUID, TOptional<XblGuid> correlationId)> OnSessionCreatedCallback;
	std::function<void()> OnServiceDisconnectedCallback;
	std::function<void(const FUniqueNetId& XUID, ESessionFailure::Type)> OnMemberLeftCallback;

	FOnlineGameSessionBase GameSessionBase;
	int32 HostingPlayerNum = -1;
	TSharedPtr<FNamedOnlineSession> HostedSession;
};
