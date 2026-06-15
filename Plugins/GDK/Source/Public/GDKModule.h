#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FGDKModule : public IModuleInterface
{
private:

public:

	FGDKModule()
	{}

	virtual ~FGDKModule() {}

	// IModuleInterface

	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}
	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

	virtual bool SupportsAutomaticShutdown() override
	{
		return false;
	}
};