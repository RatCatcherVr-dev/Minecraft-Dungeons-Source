#pragma once

#include "CoreMinimal.h"
#include <ticker.h>
#include <string>
#include <functional>
#include "GDKInterfaceDefs.h"

/**
 * GDKDungeons - Implementation of GDK API surface
 */
class GDK_API FGDKDungeonsInterface : public IModuleInterface
#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	, public FTickerObjectBase
#endif
{

public:
	static FGDKDungeonsInterface& Get() {
		return FModuleManager::LoadModuleChecked<FGDKDungeonsInterface>("GDK");
	}

	static bool IsAvailable() {
		return FModuleManager::Get().IsModuleLoaded("GDK");
	}

	FGDKDungeonsInterface() {}

	FGDKDungeonsInterface(const FGDKDungeonsInterface&) {}

	virtual ~FGDKDungeonsInterface() {}

#if defined(GDK_API_ENABLED) && GDK_API_ENABLED == 1
	HRESULT InitializeGDK();
	HRESULT UninitializeGDK();
	HRESULT RegisterForInvites(void *context, XGameInviteEventCallback *callback, GDKTaskQueueRegistrationToken* token);
	bool UnregisterForInvites(GDKTaskQueueRegistrationToken token, bool wait);

	HRESULT CreateStoreContext();
	HRESULT QueryLicenseTokenAsync(const char** productIds, size_t productIdsCount, const char* customDeveloperString, std::function<void(std::string)> tokenCallback);

	// FTickerObjectBase
	virtual bool Tick(float DeltaTime) override;

private:
	GDKTaskQueueHandle queueHandle = nullptr;
	bool isInitialized = false;
	XStoreContextHandle storeContext = nullptr;
#endif
};

typedef TSharedPtr<FGDKDungeonsInterface, ESPMode::ThreadSafe> FGDKDungeonsInterfacePtr;
