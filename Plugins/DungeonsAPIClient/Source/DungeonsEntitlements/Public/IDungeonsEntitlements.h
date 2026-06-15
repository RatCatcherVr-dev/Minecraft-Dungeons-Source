#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "EntitlementsClient.h"
#include <Anticheat.hpp>

class IDungeonsEntitlements : public IModuleInterface {

public:
	static IDungeonsEntitlements& Get() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsEntitlements";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::LoadModuleChecked<IDungeonsEntitlements>(moduleName);
	}

	static bool IsAvailable() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsEntitlements";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::Get().IsModuleLoaded(moduleName);
	}

	virtual std::shared_ptr<EntitlementsClient> Entitlements() = 0;
};
