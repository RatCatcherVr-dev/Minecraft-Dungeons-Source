#include "Dungeons.h"
#include "Terrain.h"
#include "world/level/BlockSource.h"

terrain::Block Terrain::getBlock(const TerrainPos& pos) const {
	return block.get(pos.x, pos.y);
}

terrain::Block Terrain::getBlockMag(const TerrainPos& pos) const {
	return block.getMag(pos.x, pos.y);
}

terrain::Height Terrain::getHeight(const TerrainPos& pos) const {
	return height.get(pos.x, pos.y);
}

terrain::Height Terrain::getHeightMag(const TerrainPos& pos) const {
	return height.getMag(pos.x, pos.y);
}

terrain::Type Terrain::getType(const TerrainPos& pos) const {
	return type.get(pos.x, pos.y);
}

terrain::Type Terrain::getTypeMag(const TerrainPos& pos) const {
	return type.getMag(pos.x, pos.y);
}

terrain::Reveal Terrain::getReveal(const TerrainPos& pos) const {
	return reveal.get(pos.x, pos.y);
}

terrain::Reveal Terrain::getRevealMag(const TerrainPos& pos) const {
	return reveal.getMag(pos.x, pos.y);
}

namespace terrainUtil {
	bool isLeaves(const BlockID& id) {
		static const auto leavesId = Block::mLeaves->getId();
		static const auto leaves2Id = Block::mLeaves2->getId();

		return id == leavesId ||
			id == leaves2Id;
	}

	bool isLog(const BlockID& id) {
		static const auto logId = Block::mLog->getId();
		static const auto log2Id = Block::mLog2->getId();

		return id == logId ||
			id == log2Id;
	}

	bool isVegetation(const BlockID& id) {
		static const std::array<bool, 256> vegetation = [] {
			std::array<bool, 256> tmp{ false };

			for (auto&& id : {
				Block::mReeds->getId(),
				Block::mTallgrass->getId(),
				Block::mDeadBush->getId(),
				Block::mYellowFlower->getId(),
				Block::mRedFlower->getId(),
				Block::mBrownMushroom->getId(),
				Block::mRedMushroom->getId()
				}) {
				tmp[id] = true;
			}

			return tmp;
		}();

		return vegetation[id];
	}

	std::pair<BlockID, int> findTopmost(BlockSource* blockSource, int x, int z, int minY, int maxY) {
		bool leavesState = false;

		for (int y = maxY; y >= minY; y--) {
			const auto id = blockSource->getBlockID(x, y, z);

			if (isLeaves(id)) {
				leavesState = true;
				continue;
			}

			if (leavesState && isLog(id)) {
				continue;
			}

			if (isVegetation(id)) {
				continue;
			}

			if (id != BlockID::AIR && !isLeaves(id)) {
				return { id, y };
			}
		}

		return { BlockID::AIR, (maxY + minY) / 2 };
	}
}

void Terrain::fill(const game::Tiles& tiles, BlockSource* blockSource) {
	for (auto&& tile : tiles.getTiles()) {
		const auto tileBounds = tile->tilePlacement().bounds();
		const auto regionPlane = tile->tilePlacement().regionPlane();
		
		for (auto&& cell : regionPlane) {
			const auto x = cell.position.x;
			const auto y = cell.position.z;			

			type.set(x, y, cell.value);

			{
				const auto topmost = terrainUtil::findTopmost(blockSource, x, y, tileBounds.minInclusive.y, tileBounds.maxExclusive.y - 1);

				block.set(x, y, topmost.first);
				height.set(x, y, topmost.second);
			}
		}
	}
}

void Terrain::fillDoors(const std::vector<BlockCuboid>& doors) {
	for (const auto& door : doors) {
		for (auto z = door.minInclusive.z; z < door.maxExclusive.z; z++) {
			for (auto x = door.minInclusive.x; x < door.maxExclusive.x; x++) {
				type.set(x, z, TerrainCell::boundary);
			}
		}
	}
}

int Terrain::revealAround(const TerrainPos& pos, float radiusScale, const TerrainPos& delta) {
	if (getReveal(pos) == RevealCell::Covered) {
		return 0;
	}

	const signed char revealRadiusOuter = static_cast<int>(radiusScale * Terrain::baseRevealRadiusOuter);
	const signed char revealRadiusInner = static_cast<int>(radiusScale * Terrain::baseRevealRadiusInner);

	int fullRevealCount = 0;

	const signed char lineIndexStart = delta.y > 0 ? 0 : -revealRadiusOuter;
	const signed char lineIndexEnd = delta.y < 0 ? 0 : revealRadiusOuter;

	const signed char columnIndexStart = delta.x > 0 ? 0 : -revealRadiusOuter;
	const signed char columnIndexEnd = delta.x < 0 ? 0 : revealRadiusOuter;

	for (signed char lineIndex = lineIndexStart; lineIndex < lineIndexEnd; lineIndex++) {
		for (signed char columnIndex = columnIndexStart; columnIndex < columnIndexEnd; columnIndex++) {
			if (lineIndex * lineIndex + columnIndex * columnIndex > revealRadiusOuter * revealRadiusOuter) {
				continue;
			}

			const TerrainPos terrainPos { pos.x + columnIndex, pos.y + lineIndex };

			const auto& current = reveal.get(terrainPos.x, terrainPos.y);

			if (current == RevealCell::Covered) {
				continue;
			}

			if (current == RevealCell::FullReveal) {
				if (lineIndex == 0 && columnIndex == 0) {
					reveal.set(terrainPos.x, terrainPos.y, RevealCell::Covered);
				}
			} else if (lineIndex * lineIndex + columnIndex * columnIndex > revealRadiusInner * revealRadiusInner) {
				reveal.set(terrainPos.x, terrainPos.y, RevealCell::PartialReveal);
			} else {
				reveal.set(terrainPos.x, terrainPos.y, RevealCell::FullReveal);
				fullRevealCount++;
			}
		}
	}

	dirty = true;
	
	return fullRevealCount;
}

void Terrain::revealPartialAround(const TerrainPos& pos, float radiusScale, const TerrainPos& delta) {
	if (getReveal(pos) == RevealCell::Covered) {
		return;
	}

	const signed char revealRadiusOuter = static_cast<int>(radiusScale * Terrain::baseRevealRadiusOuter);

	const signed char lineIndexStart = delta.y > 0 ? 0 : -revealRadiusOuter;
	const signed char lineIndexEnd = delta.y < 0 ? 0 : revealRadiusOuter;

	const signed char columnIndexStart = delta.x > 0 ? 0 : -revealRadiusOuter;
	const signed char columnIndexEnd = delta.x < 0 ? 0 : revealRadiusOuter;

	for (signed char lineIndex = lineIndexStart; lineIndex < lineIndexEnd; lineIndex++) {
		for (signed char columnIndex = columnIndexStart; columnIndex < columnIndexEnd; columnIndex++) {
			if (lineIndex * lineIndex + columnIndex * columnIndex > revealRadiusOuter * revealRadiusOuter) {
				continue;
			}

			const TerrainPos terrainPos { pos.x + columnIndex, pos.y + lineIndex };

			if (reveal.get(terrainPos.x, terrainPos.y) == RevealCell::Fog) {
				reveal.set(terrainPos.x, terrainPos.y, RevealCell::PartialReveal);
			}
		}
	}

	dirty = true;
}

void Terrain::clear() {
	block.clear();
	height.clear();
	type.clear();
	reveal.clear();
}

bool Terrain::wasDirty() {
	if (dirty) {
		dirty = false;
		return true;
	}
	return false;
}
