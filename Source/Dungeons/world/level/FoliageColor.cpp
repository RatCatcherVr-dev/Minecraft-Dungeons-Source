/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "world/level/FoliageColor.h"
//#include "Core/Debug/DebugUtils.h"
#include "world/level/LevelConstants.h"
#include "util/Random.h"
#include "world/level/biome/Biome.h"
#include "world/level/BlockPos.h"
#include "world/level/BlockSource.h"
#include "world/level/LevelConstants.h"
#include "world/phys/Vec3.h"
#include "util/Math.h"
#include "client/renderer/texture/TextureData.h"
//#include "Renderer/HAL/Enums/TextureFormat.h"

const Color FoliageColor::GRASS_TOP_LEFT = Color::fromARGB(0x47d033);
const Color FoliageColor::GRASS_TOP_RIGHT = Color::fromARGB(0x6cb493);
const Color FoliageColor::GRASS_BOTTOM_LEFT = Color::fromARGB(0xbfb655);
const Color FoliageColor::GRASS_BOTTOM_RIGHT = Color::fromARGB(0x80b497);

const Color FoliageColor::FOLIAGE_TOP_LEFT = Color::fromARGB(0x1ABF00);
const Color FoliageColor::FOLIAGE_TOP_RIGHT = Color::fromARGB(0x4BA875);
const Color FoliageColor::FOLIAGE_BOTTOM_LEFT = Color::fromARGB(0xAEA42A);
const Color FoliageColor::FOLIAGE_BOTTOM_RIGHT = Color::fromARGB(0x60A17B);

std::vector<int32_t> FoliageColor::foliageColors;
std::vector<int32_t> FoliageColor::foliageBirchColors;
std::vector<int32_t> FoliageColor::foliageEvergreenColors;
std::vector<int32_t> FoliageColor::grassColors;

int FoliageColor::_index(float rain, float temp) {
	rain *= temp;
	const int x = Math::clamp((int)((1 - temp) * 255), 0, 255);
	const int y = Math::clamp((int)((1 - rain) * 255), 0, 255);

	return (y << 8) | x;
}

int FoliageColor::_getX(float rain, float temp) {
	return Math::clamp((int) ((1 - temp) * 255), 0, 255);
}

int FoliageColor::_getY(float rain, float temp) {
	rain *= temp;
	return Math::clamp((int)((1 - rain) * 255), 0, 255);
}

int FoliageColor::getMapGrassColor(float temp, float rain) {
	return WHITE;
}

int FoliageColor::getMapFoliageColor(float temp, float rain) {
	return WHITE;
}

int FoliageColor::getGrassColor(float temp, float rain) {
	return WHITE;
}

int FoliageColor::getFoliageColor(float temp, float rain) {
	return WHITE;
}

int FoliageColor::getEvergreenColor(float temp, float rain) {
	return WHITE;
}

int FoliageColor::getMapEvergreenColor(float temp, float rain) {
	return WHITE;
}

int FoliageColor::getBirchColor(float temp, float rain) {
	return WHITE;
}

int FoliageColor::getMapBirchColor(float temp, float rain) {
	return WHITE;
}

int FoliageColor::getDefaultColor() {
	return WHITE;
}

int FoliageColor::_toRGB(int abgr) {
	return (abgr & 0xff) << 16 | (abgr & 0xff00) | (abgr & 0xff0000) >> 16;
}

int FoliageColor::getGrassColor(BlockSource& source, const BlockPos& pos) {
	return WHITE;
}

int FoliageColor::getMapGrassColor(BlockSource& source, const BlockPos& pos) {
	return WHITE;
}

static void extractColorData(const TextureData& texture, std::vector<int32_t>& palette) {
	const uint8_t* textureData = texture.getData();

	int pixelCount = texture.getWidth() * texture.getHeight();

	switch (texture.getTextureFormat()) {

	case EPixelFormat::PF_B8G8R8A8:
		palette.resize(pixelCount);
		for (int pixelIndex : range(pixelCount)) {
			int32_t convertedValue = 0xFF000000 | (textureData[2] << 16) | (textureData[1] << 8) | textureData[0];
			palette[pixelIndex] = convertedValue;
			textureData += 4;
		}
		break;
		
	default:
		break;
	}
}

void FoliageColor::_setPalette(const TextureData& texture, std::vector<int32_t>& palette) {
	std::vector<int32_t> temp;
	extractColorData(texture, temp);

	if (temp.size() >= PALETTE_SIZE * PALETTE_SIZE) {
		temp.resize(PALETTE_SIZE*PALETTE_SIZE);
		palette.resize(PALETTE_SIZE *PALETTE_SIZE);
		std::copy(temp.begin(), temp.end(), palette.begin());
	}
}

void FoliageColor::setFoliageColorPalette(const TextureData& texture) {
	_setPalette(texture, foliageColors);
}

void FoliageColor::setFoliageBirchColorPalette(const TextureData& texture) {
	_setPalette(texture, foliageBirchColors);
}

void FoliageColor::setFoliageEvergreenColorPalette(const TextureData& texture) {
	_setPalette(texture, foliageEvergreenColors);
}

void FoliageColor::setGrassColorPalette(const TextureData& texture) {
	_setPalette(texture, grassColors);
}

void FoliageColor::buildGrassColor(const BlockPos& min, const BlockPos& max, BlockSource& source, Random& random) {
	// Set grass color for block
	for (BlockPos p = min; p.x < max.x; ++p.x) {
		for (p.z = min.z; p.z < max.z; ++p.z) {
			source.setGrassColor(WHITE, p);
		}
	}
}
