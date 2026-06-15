#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "EntitlementsClient.h"
#include "IDungeonsAuth.h"
#include <Themida/Anticheat.hpp>
#include "TrialsClientManager.h"
#include "DungeonsClientInfo.h"
#include "LiveOpsClient.h"

class FTimerManager;

class DUNGEONSAPICLIENT_API IDungeonsAPIClient : public IModuleInterface {

public:
	static IDungeonsAPIClient& Get() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsAPIClient";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::LoadModuleChecked<IDungeonsAPIClient>(moduleName);
	}

	static bool IsAvailable() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsAPIClient";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::Get().IsModuleLoaded(moduleName);
	}

	virtual std::shared_ptr<DungeonsAuthenticator> Auth() = 0;
	virtual std::shared_ptr<TrialsClientManager> Trials() = 0;
	virtual std::shared_ptr<EntitlementsClient> Entitlements() = 0;
	virtual std::shared_ptr<LiveOpsClient> LiveOps() = 0;

	virtual void Init(const DungeonsClientInfo&, FTimerManager&) = 0;
};
