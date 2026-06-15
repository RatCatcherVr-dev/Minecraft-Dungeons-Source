#include "Dungeons.h"

#include "world/level/biome/BiomeInclude.h"
#include "world/level/FoliageColor.h"
#include "world/entity/EntityTypes.h"
#include "util/KeyValueInput.h"
#include "world/level/ChunkBlockPos.h"
#include "world/level/block/Block.h"

const Color Biome::DEFAULT_WATER_COLOR = Color::fromARGB(0x0098e8ff);
const Color Biome::DEFAULT_UNDERWATER_COLOR = Color::fromARGB(0x00050532);

const float Biome::RAIN_TEMP_THRESHOLD = 0.15f;

BiomePtr Biome::ocean;
Biome* Biome::DEFAULT;

/*static*/
Biome* Biome::mBiomes[Biome::BIOMES_COUNT] {
	nullptr
};

void Biome::initBiomes() {
	(ocean = make_unique<OceanBiome>(0))->setColor(0x000070).setName("Ocean").setDepthAndScale(-1.0f, 0.1f);
	DEFAULT = ocean.get();
}

Biome::Biome( int id, BiomeType biomeType)
	: mMaterial(((const Block*)Block::mDirt)->mID)
	, mLeafColor(0x4EE031)
	, mId(id)
	, mBiomeType(biomeType)
{
	setDepthAndScale(0.1f, 0.2f);

	mBiomes[id] = this;
}

Biome::~Biome() {

}

Biome& Biome::setName( const std::string& name ){
	mName = name;
	return *this;
}

Biome& Biome::setColor( int color ){
	return setColor(color, false);
}

Biome& Biome::setColor(int color, bool oddColor) {
	color = 0xff000000 | ((color & 0x00ff0000) >> 16) | ((color & 0x0000ff00)) | ((color & 0x000000ff) << 16);
	return *this;
}

Biome& Biome::setDepthAndScale(float depth, float scale) {
	mDepth = depth;
	mScale = scale;
	return *this;
}

Biome* Biome::getBiome( int id) {
	return getBiome(id, nullptr);
}

Biome* Biome::getBiome(int id, Biome* defaultBiome) {

	if(id < 0 || id > BIOMES_COUNT) {
		//LOGW("Biome ID is out of bounds: %d, defaulting to 0 (Ocean)\n", id);
		return ocean.get();
	}
	Biome* biome = mBiomes[id];
	if(biome == nullptr) {
		return defaultBiome;
	}
	return biome;
}

Color Biome::getSkyColor( float temp ){
	temp /= 3.f;
	if (temp < -1) {
		temp = -1;
	}
	if (temp > 1) {
		temp = 1;
	}
	return Color::fromHSB(224.f / 360.0f - temp * 0.05f, 0.50f + temp * 0.1f, 1.0f);
}

int Biome::getFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getFoliageColor(temp, rain);
}

int Biome::getBirchFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getBirchColor(temp, rain);
}

int Biome::getEvergreenFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getEvergreenColor(temp, rain);
}

int Biome::getMapFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getMapFoliageColor(temp, rain);
}

int Biome::getMapBirchFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getMapBirchColor(temp, rain);
}

int Biome::getMapEvergreenFoliageColor() {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getMapEvergreenColor(temp, rain);
}

float Biome::getDownfall() const {
	return mDownfall;
}

FullBlock Biome::getRandomFlowerTypeAndData(Random& random, const BlockPos& pos){
	return Block::mYellowFlower->mID;
}

int Biome::getGrassColor( const BlockPos& pos ) {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getGrassColor(temp, rain);
}

int Biome::getMapGrassColor(const BlockPos& pos) {
	float temp = Math::clamp(getTemperature(), 0.0f, 1.0f);
	float rain = Math::clamp(getDownfall(), 0.0f, 1.0f);

	return FoliageColor::getMapGrassColor(temp, rain);
}

Biome::BiomeType Biome::getBiomeType() {
	return mBiomeType;
}

bool Biome::isHumid(){
	return mDownfall > .85f;
}

float Biome::getTemperature() {
	return mTemperature;
}
