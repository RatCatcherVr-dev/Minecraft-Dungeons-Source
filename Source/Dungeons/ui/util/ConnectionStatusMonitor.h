#pragma once

#include "ConnectionStatus.h"
#include "Function.h"
#include "IDelegateInstance.h"

class UGameInstance;

/**
 * Helper class for managing binding to the LiveOps delegate that communicates
 * connection status changes.
 */
class ConnectionStatusMonitor {
public:
	using Callback = TFunction<void(EMinecraftAPIConnectionStatus)>;

	ConnectionStatusMonitor();

	void Start(UGameInstance* gameInstance, const Callback& onChange);
	void Stop();
	void TriggerManually() const;

private:
	UGameInstance* GameInstance;
	Callback OnChange;
	FDelegateHandle OnStatusChangedHandle;
};
