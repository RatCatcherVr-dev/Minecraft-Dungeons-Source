#include "Dungeons.h"
#include "MissionThemeRow.h"


TArray<TSoftObjectPtr<UTexture2D>> FMissionThemeRow::getSoftTexturePtrs() const {
	TArray<TSoftObjectPtr<UTexture2D>> paths;
	Marker.appendTexturePaths(paths);
	Badge.appendTexturePaths(paths);
	return paths;
}