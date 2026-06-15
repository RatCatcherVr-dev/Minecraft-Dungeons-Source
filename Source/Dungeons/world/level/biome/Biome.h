#pragma once

#include "common_header.h"

#include "CommonTypes.h"

#include "legacy/Core/Math/Color.h"
#include "legacy/Core/Utility/buffer_span.h"
#include "util/NibblePair.h"

class BlockSource;
class Random;
class BlockPos;
class BlockVolume;

class Biome;
typedef std::unique_ptr<Biome> BiomePtr;

class Biome {
public:
	static const Color DEFAULT_WATER_COLOR, DEFAULT_UNDERWATER_COLOR;

	static const int BIOMES_COUNT = 256;
	static const float RAIN_TEMP_THRESHOLD;

	enum class BiomeType {
		Beach,
		Desert,
		ExtremeHills,
		Flat,
		Forest,
		Hell,
		Ice,
		Jungle,
		Mesa,
		MushroomIsland,
		Ocean,
		Plain,
		River,
		Savanna,
		StoneBeach,
		Swamp,
		Taiga,
		TheEnd
	};

	enum class BiomeTempCategory {
		OCEAN,
		COLD,
		MEDIUM,
		WARM
	};

	static BiomePtr ocean;
	static Biome* DEFAULT;

protected:

	Biome(int id, BiomeType biomeType);
	
	virtual Biome& setColor(int color);
	virtual Biome& setColor(int color, bool oddColor);

public:
	Biome& setDepthAndScale(float depth, float scale);

	bool canHaveSnowfall() const {
		return false;
	}

	Biome& setName(const std::string& name);

	static void initBiomes();

	virtual ~Biome();

	virtual float getTemperature();

	static Biome* getBiome(int id);
	static Biome* getBiome(int id, Biome* defaultBiome);

	virtual Color getSkyColor(float temp);

	virtual int getFoliageColor();
	virtual int getBirchFoliageColor();
	virtual int getEvergreenFoliageColor();
	virtual int getMapFoliageColor();
	virtual int getMapBirchFoliageColor();
	virtual int getMapEvergreenFoliageColor();

	float getDownfall() const;

	virtual FullBlock getRandomFlowerTypeAndData(Random& random, const BlockPos& pos);
	virtual void decorate(BlockSource* source, Random& random, const BlockPos& origin, bool legacy, float worldLimit) {}
	virtual int getGrassColor(const BlockPos& pos);
	virtual int getMapGrassColor(const BlockPos& pos);

	BiomeType getBiomeType();

	virtual bool isHumid();

	bool operator==(const BiomePtr& b) const {
		return b->mId == mId;
	}

	bool operator!=(const BiomePtr& b) const {
		return b->mId != mId;
	}

	std::string mName;
	BlockID mMaterial;
	int mLeafColor;

	float mTemperature = 0.5;
	float mDownfall = 0.5;
	
	float mDepth;
	float mScale;
	Color mWaterColor = DEFAULT_WATER_COLOR, mUnderWaterColor = DEFAULT_UNDERWATER_COLOR;
	bool mRain = true;
	BiomeType mBiomeType;
	int mId;
protected:
	static Biome* mBiomes[BIOMES_COUNT];
};
