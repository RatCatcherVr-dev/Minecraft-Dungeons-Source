#pragma once

#include "LevelFileCommonTypes.h"
#include "IoObjectiveTypes.h"

namespace io {

using WeightedTileId = WeightedId;


struct Teleport {
	std::string door;
	TOptional<RegionLocator> target;
	TOptional<std::vector<WeightedId>> dungeons;
	TOptional<FString> object;

	bool isJumpToExistingPoint() const;
	bool isJumpToNewDungeon() const;
	bool hasDungeonId(const CaseInsensitiveId&) const;
};


struct PropId : public WeightedId {
	using WeightedId::WeightedId;
	std::vector<RegionLocator> targets;
};


struct Tile : public WeightedId {
	Tile() = default; // using WeightedId::WeightedId; doesn't work for some weird reason -- probably a bug in my MSVC version
	Tile(const std::string& id, float weight = 1.0f);
	Tile(const WeightedId&);

	std::string objectId;

	struct Metadata {
		Rotations rotations;
		std::string entryDoor;
		std::vector<std::string> exitDoors;
		std::vector<Challenge> challenges;
		std::vector<std::string> challengesNotFound;
		std::vector<Teleport> teleports;
		std::vector<std::string> unlockKeys;
		bool isGoal = false;
	};
	Metadata metadata;

	bool operator<(const Tile& rhs) const;
};

}
