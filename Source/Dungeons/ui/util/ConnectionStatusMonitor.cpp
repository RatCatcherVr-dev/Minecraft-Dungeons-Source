#include "ConnectionStatusMonitor.h"
#include "online/seasons/LiveOps.h"
#include "online/sessions/OnlineUtil.h"

ConnectionStatusMonitor::ConnectionStatusMonitor()
	: GameInstance(nullptr) {
}

void ConnectionStatusMonitor::Start(UGameInstance* gameInstance, const Callback& onChange) {
	if (OnStatusChangedHandle.IsValid()) {
		checkNoEntry();
		Stop();
	}

	GameInstance = gameInstance;
	OnChange = onChange;

	if (GameInstance) {
		OnStatusChangedHandle = online::getLiveOps(GameInstance)->OnConnectionStatusChanged.AddLambda(OnChange);
	}
}

void ConnectionStatusMonitor::Stop() {
	if (GameInstance) {
		online::getLiveOps(GameInstance)->OnConnectionStatusChanged.Remove(OnStatusChangedHandle);
	}

	GameInstance = nullptr;
	OnChange = {};
	OnStatusChangedHandle.Reset();
}

void ConnectionStatusMonitor::TriggerManually() const {
	check(GameInstance);
	check(OnChange);

	if (GameInstance && OnChange) {
		OnChange(online::getLiveOps(GameInstance)->GetConnectionStatus());
	}
}
