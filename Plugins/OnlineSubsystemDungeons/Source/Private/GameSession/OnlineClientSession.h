#pragma once

#include "OnlineGameSession.h"
#include "OnlineGameSessionBase.h"

class FOnlineIdentityDungeons;

class FOnlineClientSession : public IOnlineGameSession {
public:
	enum class EJoinResult {
		Success,
		NotFound,
		GenericFail,
		SessionIsFull
	};

	static TUniquePtr<IOnlineGameSession> JoinSession(
		FOnlineIdentityDungeons* OnlineIdentity,
		XblUserHandle UserHandle,
		const FString& HandleId,
		XTaskQueueHandle TaskQueueHandle,
		uint64_t HostXUID,
		std::function<void(EJoinResult)> OnJoinedCallback,
		std::function<void()> OnHostChangedCallback,
		std::function<void()> OnServiceDisconnectedCallback
	);

	void HandleEvent(const XblMultiplayerEvent&) override;
	void Tick(float DeltaTime) override;

private:
	explicit FOnlineClientSession(FOnlineIdentityDungeons* OnlineIdentity)
		: GameSessionBase(OnlineIdentity)
	{}

	void HandleJoin(const XblMultiplayerEvent&);
	void CallCallbackOnce(std::function<void()>);

	FOnlineGameSessionBase GameSessionBase;

	std::function<void()> OnHostChangedCallback;
	std::function<void(EJoinResult)> OnJoinedCallback;
	std::function<void()> OnServiceDisconnectedCallback;
	uint64_t HostXUID;
	bool hasCallbackBeenCalled = false;

	bool bIsJoiningSession = false;
	float TimeToJoinSession = 0;
};

