#pragma once

#include "world/level/BlockPos.h"

class TilePlacement;
struct Placement;

namespace game {
struct LevelDef;

namespace sublevel {
class TileLoader;
}
}

namespace decor {

DUNGEONS_API FString filenameFor(const FString& group, const FString& tile);
DUNGEONS_API FString filenameForActor(const FString& group, const FString& actor);
DUNGEONS_API FString filenameForEnvironmental(const FString& group, const FString& actor);
DUNGEONS_API FString pathForSublevels(const FString& mission, const FString& group);
//UPackage* packageFor(const TArray<FString>& groups, const FString& tile);
DUNGEONS_API TOptional<FTransform> getTileOriginInDecorSpace(AActor& root);

FTransform placementToTransform(const Placement&, BlockPos);

AActor* placeDecorActor(UWorld&, const TilePlacement&, const TSubclassOf<AActor>&);
TArray<AActor*> placeTileDecors(UWorld&, const game::LevelDef&);
void placeDecorActors(UWorld&, const game::LevelDef&);
void placeDecorSubLevels(UWorld&, const game::LevelDef&);

void createDecorSubLevels(const game::LevelDef&, game::sublevel::TileLoader&);


}
