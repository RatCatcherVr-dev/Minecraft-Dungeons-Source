#include "Dungeons.h"
#include "InternalPositionCorrectors.h"
#include "game/Conversion.h"
#include "game/Game.h"
#include "game/level/GameTiles.h"
#include "game/util/LocationQuery.h"
#include "lovika/BlockPosTransform.h"
#include "lovika/Grid2.h"
#include "world/entity/MobTags.h"


namespace game { namespace mobspawn { namespace positioncorrectors { namespace internal {

TOptional<FTransform> correctPositionByLevelTrace(UWorld& world, FTransform transform, bool ignorePlayers) {
	const auto uncorrectedTranslation = transform.GetTranslation();
	if (const auto ground = locationquery::findGround(world, uncorrectedTranslation, ignorePlayers)) {
		transform.SetLocation({ uncorrectedTranslation.X, uncorrectedTranslation.Y, ground.GetValue() });
	}
	if (!ignorePlayers && locationquery::isOverlappingPlayers(world, transform.GetTranslation() + FVector{ 0.f, 0.f, 100.f })) {
		return {};
	}
	return transform;
}

// @note: I don't know what this code is supposed to do, but for now I only
//        refactor, and try to not change behavior so it's duplicated.
TOptional<float> adjustSpawnHeight(TileRef tile, const BlockPos& blockPos) {
	auto height = tile.tileArea().heights.get(blockPos);

	// enum Obstructed: Disallow , [MoveUp, Always]
	if (std::abs(height - blockPos.y) >= 1) {
		auto&& blocks = tile.tile().blocks();
		auto transform = blockpostransform::_invert(tile.tilePlacement().blockPosTransform());

		BlockPos testPos(blockPos);
		for (int i = 0; i < 3; ++i) {
			if (blocks.getBlockId(transform(testPos)) != BlockID::AIR) {
				return {};
			}
			testPos.moveUp();
		}
	}
	return blockPos.y;
}

TOptional<FTransform> correctPositionByTileData(const Game& game, EntityType type, FTransform transform) {
	Vec3 blockPos = conversion::ueToPos(transform.GetLocation());

	if (TilePtr tile = game.tiles().getTile(blockPos)) {
		if (auto y = internal::adjustSpawnHeight(*tile, blockPos)) {
			blockPos.y = std::max(blockPos.y, static_cast<float>(y.GetValue()));
			transform.SetLocation(conversion::posToUe(blockPos));
		} else {
			return {};
		}
	}
	// still needed?
	if (hasMobTag(type, MobTags::HashTag_Miniboss)) {
		transform.SetRotation(FQuat(FRotator(0, 0, 0))); // I'm assuming this is the same as a zero FQuat, but just to be sure
	}
	return transform;
}

}}}}
