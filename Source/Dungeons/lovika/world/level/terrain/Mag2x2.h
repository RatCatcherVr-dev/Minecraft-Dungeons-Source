#pragma once

#include "Dungeons.h"
#include "TerrainTypes.h"
#include "TerrainGrid.h"
#include "TerrainPos.h"

struct BlockMag2x2 {
	static void update(const TerrainGrid<terrain::Block>& data, int x, int y, TerrainGrid<terrain::Block>& mag);
};

struct HeightMag2x2 {
	static void update(const TerrainGrid<terrain::Height>& data, int x, int y, TerrainGrid<terrain::Height>& mag);
};

struct TerrainMag2x2 {
	static void update(const TerrainGrid<terrain::Type>& data, int x, int y, TerrainGrid<terrain::Type>& mag);
};

struct RevealMag2x2 {
	static void update(const TerrainGrid<terrain::Reveal>& data, int x, int y, TerrainGrid<terrain::Reveal>& mag);
};