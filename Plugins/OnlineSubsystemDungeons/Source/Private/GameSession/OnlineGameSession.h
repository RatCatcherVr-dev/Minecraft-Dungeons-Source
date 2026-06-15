#pragma once

struct XblMultiplayerEvent;

class IOnlineGameSession {
public:
	virtual ~IOnlineGameSession() {}

	virtual void HandleEvent(const XblMultiplayerEvent&) = 0;
	virtual void Tick(float DeltaTime) = 0;
};
