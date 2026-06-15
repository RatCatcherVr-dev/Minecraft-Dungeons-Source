#pragma once

#include "CommonTypes.h"
#include "TerrainTypes.h"
#include "game/level/GameTiles.h"
#include "TerrainPos.h"
#include "Mag2x2.h"
#include "TerrainGridMag.h"

namespace game { class Tiles; }

class DUNGEONS_API Terrain {
public:
	Terrain()
		: block { 0 }
		, height { 0 }
		, type { TerrainCell::boundary }
		, reveal { RevealCell::Fog }
	{};

	terrain::Block getBlock(const TerrainPos&) const;
	terrain::Block getBlockMag(const TerrainPos&) const;

	terrain::Height getHeight(const TerrainPos&) const;
	terrain::Height getHeightMag(const TerrainPos&) const;

	terrain::Type getType(const TerrainPos&) const;
	terrain::Type getTypeMag(const TerrainPos&) const;	

	void fill(const game::Tiles&, BlockSource*);
	void fillDoors(const std::vector<BlockCuboid>&);
	
	terrain::Reveal getReveal(const TerrainPos&) const;	
	terrain::Reveal getRevealMag(const TerrainPos&) const;

	int revealAround(const TerrainPos&, float radiusScale, const TerrainPos& delta = TerrainPos(0, 0));
	void revealPartialAround(const TerrainPos&, float radiusScale, const TerrainPos& delta = TerrainPos(0, 0));

	void clear();

	bool wasDirty();

private:
	struct HeightEntry {
		int x;
		int y;
		int height;
	};

	TerrainGridMag<terrain::Block, BlockMag2x2> block;
	TerrainGridMag<terrain::Height, HeightMag2x2> height;
	TerrainGridMag<terrain::Type, TerrainMag2x2> type;
	TerrainGridMag<terrain::Reveal, RevealMag2x2> reveal;

	signed char baseRevealRadiusOuter { 70 };
	signed char baseRevealRadiusInner { 16 };

	bool dirty;
};
