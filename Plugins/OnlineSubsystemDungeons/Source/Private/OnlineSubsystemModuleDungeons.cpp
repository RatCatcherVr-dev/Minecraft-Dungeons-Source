#include "PCHOnlineDungeonsSubsystem.h"
#include "OnlineSubsystemModuleDungeons.h"
#include "OnlineSubsystemDungeons.h"
#include "Misc/CommandLine.h"
#include "XmppModule.h"
#include <Engine/Engine.h>
#include "utils.h"

#if PLATFORM_PS4
#include <libsysmodule.h>
#endif

IMPLEMENT_MODULE(FOnlineSubsystemDungeonsModule, OnlineSubsystemDungeons);

/**
 * Class responsible for creating instance(s) of the subsystem
 */

static bool ShouldUseOnlineSubsystem() {
	bool inEditor = WITH_EDITOR;
	bool gameFlag = FParse::Param(FCommandLine::Get(), TEXT("game"));
	bool lanFlag = FParse::Param(FCommandLine::Get(), TEXT("lan"));

	if (lanFlag && !UE_BUILD_SHIPPING)
	{
		return false;
	}
	if (environmentConfig::HasOnlineFlag())
	{
		return true;
	}

	return (!inEditor || (inEditor && gameFlag));
}

class FOnlineFactoryDungeons : public IOnlineFactory
{
public:

	FOnlineFactoryDungeons() {}
	virtual ~FOnlineFactoryDungeons() {}
	IOnlineSubsystemPtr initializedDungeonsSubsystem = nullptr;

	virtual IOnlineSubsystemPtr CreateSubsystem(FName InstanceName)
	{
		if (initializedDungeonsSubsystem && environmentConfig::HasOnlineFlag())
		{	// Hack for making online work in editor, basically using this factory as a singleton
			return initializedDungeonsSubsystem;
		}

		FOnlineSubsystemDungeonsPtr OnlineSub = MakeShareable(new FOnlineSubsystemDungeons(InstanceName));
		if (OnlineSub->IsEnabled())
		{
			if (GEngine->IsEditor() && InstanceName == "DefaultInstance")
			{
				// Don't initialize the subsystem for the editor
			}
			else {
				if (OnlineSub->Init())
				{
					initializedDungeonsSubsystem = OnlineSub;
				}
				else
				{
					UE_LOG_ONLINE(Warning, TEXT("[SubsystemModuleDungeons] Dungeons API failed to initialize!"));
					OnlineSub->Shutdown();
					OnlineSub = NULL;
				}
			}
		}
		else
		{
			UE_LOG_ONLINE(Warning, TEXT("[SubsystemModuleDungeons] Dungeons API disabled!"));
			OnlineSub->Shutdown();
			OnlineSub = NULL;
		}
		return OnlineSub;
	}
};

void FOnlineSubsystemDungeonsModule::StartupModule()
{
	if (ShouldUseOnlineSubsystem())
	{
		DungeonsFactory = new FOnlineFactoryDungeons();
		
		// Create and register our singleton factory with the main online subsystem for easy access
		FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
		OSS.RegisterPlatformService(DUNGEONSPLATFORM, DungeonsFactory);

		/* Start XMPP 'module' - copied from Engine plugin folder */
#if PLATFORM_PS4
		if (sceSysmoduleLoadModule(SCE_SYSMODULE_RANDOM) != SCE_OK) {
			UE_LOG_ONLINE(Warning, TEXT("FAILED TO LOAD Random Module!"));
		}
#endif
		FXmppModule::StartupModule();
	}
}

void FOnlineSubsystemDungeonsModule::ShutdownModule()
{
	if (ShouldUseOnlineSubsystem())
	{
		FOnlineSubsystemModule& OSS = FModuleManager::GetModuleChecked<FOnlineSubsystemModule>("OnlineSubsystem");
		OSS.UnregisterPlatformService(DUNGEONSPLATFORM);

		delete DungeonsFactory;
		DungeonsFactory = nullptr;

		FXmppModule::ShutdownModule();
	}
}
