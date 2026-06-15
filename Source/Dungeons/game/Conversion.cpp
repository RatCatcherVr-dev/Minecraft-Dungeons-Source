#include "Dungeons.h"
#include "Conversion.h"
#include "util/Math.h"
#include "world/phys/Vec3.h"
#include "world/level/BlockPos.h"
#include "lovika/BlockCuboid.h"
#include "lovika/world/level/terrain/TerrainPos.h"

namespace conversion {

FVector posToUe(float x, float y, float z) {
	return FVector(x * Math::PE_TO_UE_UNITS, z * Math::PE_TO_UE_UNITS, y * Math::PE_TO_UE_UNITS);
}

FVector posToUe(const Vec3& pos) {
	return FVector(pos.x * Math::PE_TO_UE_UNITS, pos.z * Math::PE_TO_UE_UNITS, pos.y * Math::PE_TO_UE_UNITS);
}

FVector blockToUe(const BlockPos& pos) {
	return posToUe(Vec3(pos));
}

FBox blockCuboidToUe(const BlockCuboid& volume) {
	return FBox(blockToUe(volume.minInclusive), blockToUe(volume.maxExclusive));
}

TArray<FVector> blockCuboidCenterXZToUeArray(const BlockCuboid& blockCuboid) {
	TArray<FVector> r;
	for (const auto& bp : blockCuboid) {
		r.Add(blockCenterXZToUe(bp));
	}
	return r;
}

FVector blockCenterXZToUe(const BlockPos& pos) {
	return FVector((pos.x + 0.5f) * Math::PE_TO_UE_UNITS, (pos.z + 0.5f) * Math::PE_TO_UE_UNITS, pos.y * Math::PE_TO_UE_UNITS);
}

FVector blockCenterXYZToUe(const BlockPos& pos) {
	return posToUe(Vec3(pos) + 0.5f);
}

BlockPos ueToBlock(const FVector& pos) {
	return BlockPos(pos.X * Math::UE_TO_PE_UNITS, pos.Z * Math::UE_TO_PE_UNITS, pos.Y * Math::UE_TO_PE_UNITS);
}

BlockPos ueToBlock(const AActor& actor) {
	return ueToBlock(actor.GetActorLocation());
}

TerrainPos ueToTerrain(const FVector& pos) {
	return TerrainPos(pos.X * Math::UE_TO_PE_UNITS, pos.Y * Math::UE_TO_PE_UNITS);
}

TerrainPos ueToTerrain(const AActor& actor) {
	return ueToTerrain(actor.GetActorLocation());
}

Vec3 ueToPos(const FVector& pos) {
	return Vec3(pos.X * Math::UE_TO_PE_UNITS, pos.Z * Math::UE_TO_PE_UNITS, pos.Y * Math::UE_TO_PE_UNITS);
}

Vec3 ueToPos(float x, float y, float z) {
	return Vec3(x * Math::UE_TO_PE_UNITS, z * Math::UE_TO_PE_UNITS, y * Math::UE_TO_PE_UNITS);
}

FVector terrainToUe(const TerrainPos pos) {
	return FVector(pos.x * Math::PE_TO_UE_UNITS, pos.y * Math::PE_TO_UE_UNITS, 0.f);
}


float degreesToUe(float degrees) {
	return 270 - degrees;
}

float ueToDegrees(float degrees) {
	return 270 - degrees;
}

}
