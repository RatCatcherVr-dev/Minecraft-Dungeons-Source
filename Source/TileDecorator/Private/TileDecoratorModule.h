#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "UnrealEd.h"

class FTileDecoratorModule
	: public TSharedFromThis<FTileDecoratorModule>
	, public IModuleInterface
{
public:
	FTileDecoratorModule();
	virtual ~FTileDecoratorModule();
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	void RegisterGenericCommands();
	void RegisterTileDecoratorCommands();
	void RegisterSubLevelCommands();

	TSharedRef<SWidget> SetupMenu();
	void SetupGenericMenu(FMenuBuilder&);
	void SetupTileDecoratorMenu(FMenuBuilder&);

	void ExecuteLockFileInGit();
	void ExecuteUnlockFileInGit();
	
	TSharedRef<FUICommandList> mCommands;
	TArray<FAssetData> mSelectedAssets;

	/** Delegate to extend the content browser asset context menu. */
	TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets);
	
	static void OnEditorPreSaveWorld(uint32 SaveFlags, UWorld* World);
	static void OnEditorPostSaveWorld(uint32 SaveFlags, UWorld* World, bool bSuccess);
	static void OnEditorMapOpened(const FString& Filename, bool bLoadAsTemplate);

	
};
