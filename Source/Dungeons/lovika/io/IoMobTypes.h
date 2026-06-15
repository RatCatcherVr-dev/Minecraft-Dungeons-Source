#pragma once

#include "LevelFileCommonTypes.h"
#include <Optional.h>

enum class EntityType : uint32;

namespace io {

struct MobType {
	struct Expr {
		Expr(EntityType);
		Expr(const std::string& expr) : expr(expr) {}
		Expr(const char* expr) : expr(expr) {}
		std::string expr;
	};
	MobType(const Expr&, float weight = 1, unsigned int min = 0, unsigned int max = -1, float maxFractionOfTotal = 1);
	static MobType exactly(const Expr&, int count, float weight = 1);
	static MobType one(const Expr&, float weight = 1);

	std::string expr;
	float weight;
	unsigned int min;
	unsigned int max;
	float maxFractionOfTotal = 1; // We call this "max-percent" when we talk about it
	mutable TOptional<std::vector<EntityType>> types; // Used by game::mobspawn
	
	DifficultyRange allowedOnDifficulties;
};


struct MobGroup {
	MobGroup() = default;
	MobGroup(std::vector<MobType>, const std::string& id = "", float weight = 1);

	std::vector<MobType> types;
	std::string id;
	std::string lowerId;
	DifficultyRange allowedOnDifficulties;
	float weight = 1;
	TOptional<Interval> typesCountInterval;
};


struct StretchMobs {
	int count() const { return static_cast<int>(density); }

	float density = 1.0f;
	std::vector<MobGroup> groups;
	std::vector<std::string> excludePredicates;
};

}
