#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "core/HttpClient.h"
#include "DungeonsHTTPConfig.h"
#include <Anticheat.hpp>

class IDungeonsHTTP : public IModuleInterface {

public:
	static IDungeonsHTTP& Get() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsHTTP";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::LoadModuleChecked<IDungeonsHTTP>(moduleName);
	}

	static bool IsAvailable() {
		ANTICHEAT_PROTECT_STRINGS_BEGIN
		const FName moduleName = "DungeonsHTTP";
		ANTICHEAT_PROTECT_STRINGS_END
		return FModuleManager::Get().IsModuleLoaded(moduleName);
	}

	virtual std::shared_ptr<minecraft::api::HttpClient> Client(const std::shared_ptr<DungeonsHTTPConfig>& config) = 0;
	
};
