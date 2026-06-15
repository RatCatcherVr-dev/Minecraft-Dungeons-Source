#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "UnrealEd.h"

class FDungeonsEditorModule : public IModuleInterface {
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnPostEngineInit();
};
