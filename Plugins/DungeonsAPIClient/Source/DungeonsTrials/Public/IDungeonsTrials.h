#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "TrialsClientManager.h"
#include <Anticheat.hpp>

class IDungeonsTrials : public IModuleInterface {

public:
	static IDungeonsTrials& Get() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsTrials";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::LoadModuleChecked<IDungeonsTrials>(moduleName);
	}

	static bool IsAvailable() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsTrials";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::Get().IsModuleLoaded(moduleName);
	}

	virtual std::shared_ptr<TrialsClientManager> Trials() = 0;
};
