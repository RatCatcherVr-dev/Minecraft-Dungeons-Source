#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "LiveOpsClient.h"
#include <Anticheat.hpp>
#include <memory>

class IDungeonsLiveOps : public IModuleInterface {

public:
	static IDungeonsLiveOps& Get() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
			const FName moduleName = "DungeonsLiveOps";
		ANTICHEAT_PROTECT_STRINGS_END
			return FModuleManager::LoadModuleChecked<IDungeonsLiveOps>(moduleName);
	}

	static bool IsAvailable() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
			const FName moduleName = "DungeonsEntitlements";
		ANTICHEAT_PROTECT_STRINGS_END
			return FModuleManager::Get().IsModuleLoaded(moduleName);
	}

	virtual std::shared_ptr<LiveOpsClient> LiveOps() = 0;
};
