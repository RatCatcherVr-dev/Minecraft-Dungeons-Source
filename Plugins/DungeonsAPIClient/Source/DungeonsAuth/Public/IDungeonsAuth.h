#pragma once

#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "DungeonsAuthenticator.h"
#include <Themida/Anticheat.hpp>

class IDungeonsAuth : public IModuleInterface {

public:
	static IDungeonsAuth& Get() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsAuth";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::LoadModuleChecked<IDungeonsAuth>(moduleName);
	}

	static bool IsAvailable() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsAuth";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::Get().IsModuleLoaded(moduleName);
	}

	virtual std::shared_ptr<DungeonsAuthenticator> Auth() = 0;
};
