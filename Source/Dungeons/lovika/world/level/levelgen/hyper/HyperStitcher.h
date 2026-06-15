#pragma once

#include "lovika/io/IoHyperTypes.h"
#include "lovika/io/LevelFile.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "util/Validation.h"

typedef uint32_t RandomSeed;

namespace levelgen { namespace hajper {

struct StitchInput {
	TArray<FString> orderedLevelIds;
	TArray<FString> definitionLevelIds;
};

struct StitchResult {
	Unique<levelgen::sourcedata::SourceData> data;
	Validation issues;
};

StitchInput generateRandomStitchInput(int subMissionCount, RandomSeed, const io::HyperLevelDef&, int startAtIndex = 0);

StitchResult stitch(const StitchInput&);

}}
