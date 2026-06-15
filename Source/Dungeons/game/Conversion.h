#pragma once

class Vec3;
class BlockPos;
class Actor;
struct BlockCuboid;
struct TerrainPos;
struct FVector;

namespace conversion {

DUNGEONS_API FVector posToUe(float x, float y, float z);
DUNGEONS_API FVector posToUe(const Vec3&);
DUNGEONS_API FVector blockToUe(const BlockPos&);
DUNGEONS_API FBox blockCuboidToUe(const BlockCuboid&);
DUNGEONS_API TArray<FVector> blockCuboidCenterXZToUeArray(const BlockCuboid&);

FVector blockCenterXZToUe(const BlockPos&);
FVector blockCenterXYZToUe(const BlockPos&);

DUNGEONS_API BlockPos ueToBlock(const AActor&);
DUNGEONS_API BlockPos ueToBlock(const FVector&);

DUNGEONS_API TerrainPos ueToTerrain(const AActor&);
DUNGEONS_API TerrainPos ueToTerrain(const FVector&);

Vec3 ueToPos(const FVector&);
Vec3 ueToPos(float x, float y, float z);

DUNGEONS_API FVector terrainToUe(const TerrainPos);

float degreesToUe(float);
float ueToDegrees(float);

}
