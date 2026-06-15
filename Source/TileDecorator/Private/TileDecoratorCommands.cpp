#include "TileDecorator.h"
#include "TileDecoratorCommands.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "TileDecorator"

FTileDecoratorCommands::FTileDecoratorCommands()
	: TCommands<FTileDecoratorCommands>("TileDecorator", NSLOCTEXT("Contexts", "TileDecorator", "Tile Decorator"), NAME_None, FEditorStyle::GetStyleSetName()) {
}

void FTileDecoratorCommands::RegisterCommands() {
	UI_COMMAND(DungeonsTools, "Dungeons", "Dungeons functionality", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CopyToBlueprint, "Copy selected components to Blueprint Class..", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(CopyFrozenActorToBlueprint, "Copy frozen actor to Blueprint Class..", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SaveDecorsForSelectedTiles, "Save decors for selected tiles", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SaveDecorActorsForSelectedTiles, "Save actor(s) for selected tiles", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(SaveEnvironmentalEffects, "Save lights", "", EUserInterfaceActionType::Button, FInputChord());

	UI_COMMAND(SelectAllActorsInTile, "Select all actors in tile", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(MoveAllActorsInTileToSubLevel, "Move all actors in tile to sublevel", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(MoveAllActorsInAllTilesToSubLevels, "Move all actors in all tiles to sublevels", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ConvertCurrentActorsTileToInstanced, "Safety Function : Convert All Applicable Static Mesh Instances in Tile into Dungeons Instanced Static Meshes", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ConvertCurrentActorsTileFromInstanced, "Safety Function : Convert All Dungeons Instanced Static Meshes back into individual Static Mesh Instances", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DeleteAllDuplicatedSMs, "Safety Function : Remove all Duplicated StaticMeshes with the same type and position !!!!! USE WITH CAUTION !!!!!", "", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(ToggleRegions, "Show Regions", "", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(LockFileInGit, "Lock File...", "FTileDecoratorModule - Locks File in Git for this asset", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::L));
	UI_COMMAND(UnLockFileInGit, "Unlock File...", "FTileDecoratorModule - Unlocks File in Git for this asset", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Control, EKeys::U));

}

#undef LOCTEXT_NAMESPACE
