#pragma once

#include "CommonTypes.h"
#include "IoMobTypes.h"
#include "IoObjectiveTypes.h"
#include "IoPrefabTypes.h"
#include "LevelFileCommonTypes.h"
#include "game/difficulty/Difficulty.h"
#include "lovika/world/level/postprocess/PostProcessTypes.h"
#include <Optional.h>

namespace io {

//
// Common Types
//

using WeightedTileId = WeightedId;

struct StrayPathVariant {
	StrayPathVariant() : weight(1) {}
	StrayPathVariant(std::string id, float weight, TOptional<Interval> maxLength, TOptional<std::vector<WeightedTileId>> tiles, TOptional<std::vector<WeightedTileId>> deadEnds)
		: id(std::move(id))
		, weight(1)
		, maxLength(std::move(maxLength))
		, tiles(std::move(tiles))
		, deadEnds(std::move(deadEnds)) {
	}

	std::string id;
	float weight;
	TOptional<Interval> maxLength;
	TOptional<std::vector<WeightedTileId>> tiles;
	TOptional<std::vector<WeightedTileId>> deadEnds;

	float getWeight() const { return weight; }
};

struct StrayPathConfig {
	TOptional<float> probability;
	StrayPathVariant fallback;
	TOptional<std::vector<StrayPathVariant>> variants;
};

struct Overrides {
	TOptional<float> propDensity;
	StrayPathConfig strayPath;
	TOptional<FString> ambience;
	TOptional<FString> audioAmbience;
};

StrayPathConfig merge(const StrayPathConfig& parent, const StrayPathConfig& child);

struct StretchDef {
	std::string id;
	std::string narration;
	Overrides overrides;
	StretchMobs mobs;
	std::vector<PropId> propGroups;
	size_t dungeonIndex;
	bool allowRaidCaptains;
};

struct Stretch: public StretchDef {
	size_t indexInFile;
	int length;
	std::vector<WeightedTileId> tiles;
	TOptional<std::vector<WeightedTileId>> deadEnds;
};

struct DungeonLevelInfo {
	FString id;
	FString ambienceLevelId;
	std::vector<std::string> lowerCaseObjectGroups;
};

struct DungeonDef {
	CaseInsensitiveId id;
	size_t index;
	bool deathOutsideTile = true;
	TOptional<postprocess::Config> fillConfig;
	TOptional<FString> musicLevelOverride;
	std::string resourcePack;

	DungeonLevelInfo level; // @todo: "pointing" to some level data would be cleaner

	TOptional<std::string> dimension;
};

struct Dungeon: public DungeonDef {
	std::vector<Stretch> stretches;
	bool needsToBeValidated = false;
};

struct LevelGenConfig {
	bool requireMatchingDoors = false;
	int maxDoorWidthDifference(int unmatchedMaxDoorDifference = 999) const { return requireMatchingDoors ? 0 : unmatchedMaxDoorDifference; }
};

struct LevelHeader {
	std::string id;
	std::string loctableId;
	std::vector<ObjectGroupDef> objectGroups;
	std::vector<CaseInsensitiveId> mobGroupFileIds;
	LevelGenConfig levelGenConfig;
};

struct Level: LevelHeader {
	std::vector<Tile> tileDefs;
	std::vector<Tile> propDefs;
	std::vector<MobGroup> mobGroups;
	std::vector<Dungeon> dungeons;
	std::vector<WeightedId> initialDungeons;
	TOptional<std::string> dimension;

	struct PassThrough {
		ECharacterLoadoutType characterLoadout = ECharacterLoadoutType::DefaultLoadout;
		bool playIntro = true;
		std::vector<DungeonDef> dungeons;
		std::vector<Objective> objectives;
		std::vector<ObjectGroupDef> objectGroups;
		std::vector<MobGroup> mobGroups;
		std::vector<CaseInsensitiveId> definedLevelIds;
	} passThrough;
};

TOptional<LevelHeader> loadLevelHeader(std::istream&);

Unique<Level> loadLevel(std::istream&);
bool loadLevel(Level&, std::istream&);
bool loadObjectives(Level&, std::istream& in);
bool loadMobGroups(Level&, std::istream& in);

Unique<struct HyperLevel> loadHyperLevel(std::istream&);

}
