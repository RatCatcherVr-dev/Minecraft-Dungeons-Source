#pragma once

#include "world/level/biome/Biome.h"

class OceanBiome : public Biome {
public:
	OceanBiome(int id)
	: Biome(id, Biome::BiomeType::Ocean) {
	}
};
