#pragma once

#include "Engine.h"
#include "ModuleManager.h"

class FBotAutomationModule : public IModuleInterface {
public: // IModuleInterface

	/** Called right after the module DLL has been loaded and the module object has been created */
	virtual void StartupModule() override;

	/** Called before the module is unloaded, right before the module object is destroyed. */
	virtual void ShutdownModule() override;
};
