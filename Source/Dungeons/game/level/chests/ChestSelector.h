#pragma once

#include "ChestType.h"
#include "UnrealString.h"

namespace game { namespace tile { namespace chest {

struct ChestTypeProbability {
	
	EChestType mChestType;
	float mWeight;

	ChestTypeProbability(EChestType, float);

	float getWeight() const {
		return mWeight;
	}
};

struct ChestTypeProbabilityConfig {

	std::vector<ChestTypeProbability> mProbabilities;

	explicit ChestTypeProbabilityConfig(std::vector<ChestTypeProbability>);

	void addProbability(const ChestTypeProbability&);
};

struct ChestTypeMapping {

	EChestType mChestType;
	FString mBlueprintPath;

	ChestTypeMapping(EChestType, FString);
};

ChestTypeMapping selectRandomizedChestFor(const ChestTypeProbabilityConfig&);

void PreloadChestTypes();
	
}}}
