#pragma once
#include "Commands.h"

class FTileDecoratorCommands : public TCommands<FTileDecoratorCommands> {
public:
	FTileDecoratorCommands();

	TSharedPtr<FUICommandInfo> DungeonsTools;
	TSharedPtr<FUICommandInfo> CopyToBlueprint;
	TSharedPtr<FUICommandInfo> SaveDecorsForSelectedTiles;
	TSharedPtr<FUICommandInfo> CopyFrozenActorToBlueprint;
	TSharedPtr<FUICommandInfo> SaveDecorActorsForSelectedTiles;

	TSharedPtr<FUICommandInfo> SaveEnvironmentalEffects;

	TSharedPtr<FUICommandInfo> SelectAllActorsInTile;
	TSharedPtr<FUICommandInfo> MoveAllActorsInTileToSubLevel;
	TSharedPtr<FUICommandInfo> MoveAllActorsInAllTilesToSubLevels;
	TSharedPtr<FUICommandInfo> ConvertCurrentActorsTileToInstanced;
	TSharedPtr<FUICommandInfo> ConvertCurrentActorsTileFromInstanced;
	TSharedPtr<FUICommandInfo> DeleteAllDuplicatedSMs;
	TSharedPtr<FUICommandInfo> ToggleRegions;
	
	TSharedPtr<FUICommandInfo> LockFileInGit;
	TSharedPtr<FUICommandInfo> UnLockFileInGit;

	virtual void RegisterCommands() override;
};
