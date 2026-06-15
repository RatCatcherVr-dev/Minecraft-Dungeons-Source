#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include <Anticheat.hpp>
#include "core/MinecraftClient.h"
#include "DungeonsClientInfo.h"

class FTimerManager;

class IDungeonsClient : public IModuleInterface {

public:
	static IDungeonsClient& Get() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsClient";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::LoadModuleChecked<IDungeonsClient>(moduleName);
	}

	static bool IsAvailable() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsClient";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::Get().IsModuleLoaded(moduleName);
	}

	virtual void Init(const DungeonsClientInfo&, FTimerManager&) = 0;
	virtual shared_ptr<minecraft::api::MinecraftClient> CreateMinecraftClient() = 0;

};
