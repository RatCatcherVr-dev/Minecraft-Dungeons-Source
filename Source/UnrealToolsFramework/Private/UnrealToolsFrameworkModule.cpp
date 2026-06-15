//------------------------------------------------------------------------------
// <copyright file="UnrealToolsFrameworkModule.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "UnrealToolsFrameworkPCH.h"
#include "UnrealToolsFrameworkModule.h"

using namespace Microsoft::Internal::GamesTest::UnrealToolsFramework;

/** Main class for this tools module. */
class FUnrealToolsFrameworkModule : public IUnrealToolsFrameworkModule
{
public:
	FUnrealToolsFrameworkModule()
	{		
	}

public: // IModuleInterface

	/** Called right after the module DLL has been loaded and the module object has been created */
	virtual void StartupModule() override
	{
        // The different systems hosted by UTF are initialized and started
        // individually by the user. See UTF's docs for more information.
	}

	/** Called before the module is unloaded, right before the module object is destroyed. */
	virtual void ShutdownModule() override
	{
        // Stop all UTF systems.
        HRESULT Result = FUnrealToolsFramework::Stop();
        if (FAILED(Result))
        {
            UE_LOG(LogUToolsFramework, Warning, TEXT("Unable to Stop some services. Error: %X"), Result);
        }
	}

public: // FTickableGameObject

    /**
     * Update function, will be called on every world tick.
     * @param DeltaTime - Time elapsed since last tick.
     */
	virtual void Tick(float DeltaTime) override
	{
        HRESULT Result = FUnrealToolsFramework::Tick(DeltaTime);
        if (FAILED(Result))
        {
            UE_LOG(LogUToolsFramework, Error, TEXT("Unable to Tick some services. Error: %X"), Result);
        }
	}

    /** Return the stat id to use for this tickable. */
    virtual TStatId GetStatId() const override
    {
        RETURN_QUICK_DECLARE_CYCLE_STAT(FUnrealToolsFrameworkModule,STATGROUP_UnrealToolsFramework);
    }
};

/** Register module (Module Main Class, Module Name) */
IMPLEMENT_MODULE(FUnrealToolsFrameworkModule, UnrealToolsFramework);
