#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FOnlineSubsystemDungeonsModule : public IModuleInterface
{
private:
	class FOnlineFactoryDungeons* DungeonsFactory;

public:

	FOnlineSubsystemDungeonsModule() :
		DungeonsFactory(NULL)
	{}

	virtual ~FOnlineSubsystemDungeonsModule() {}

	// IModuleInterface

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	virtual bool SupportsDynamicReloading() override
	{
		return false;
	}

	virtual bool SupportsAutomaticShutdown() override
	{
		return false;
	}
};