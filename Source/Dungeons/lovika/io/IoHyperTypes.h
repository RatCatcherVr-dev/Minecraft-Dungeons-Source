#pragma once

#include "IoMobTypes.h"
#include "LevelFileCommonTypes.h"
#include "game/levels.h"
#include "game/item/ItemArchetypeCounts.h"
#include "game/dlc/DLCName.h"

enum class EntityType : uint32;

namespace io {

struct HyperDungeon {
	WeightedId id;
	TOptional<EDLCName> requiredDLC;
	ItemArchetypeCounts archetypeRequirements;
	std::vector<EntityType> entityTypes;
	bool isGoldDungeon = true;

	float getWeight() const { return id.getWeight(); }
	bool hasAncient() const { return !isGoldDungeon && !entityTypes.empty(); };
};

struct HyperMobGroup {
	std::vector<io::MobType> types;
};

struct HyperLevelDef {
	std::vector<WeightedId> levelIds;
	std::vector<WeightedId> definitionLevelIds;
};

struct HyperLevel: public HyperLevelDef {
	std::vector<HyperDungeon> hyperDungeons;
	std::unordered_map<ELevelNames, HyperMobGroup> hyperLevelMobs;
};

using HyperDungeonPredicate = Pred<HyperDungeon>;

}
