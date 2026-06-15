#pragma once

#include "LevelGen.h"
#include "CommonTypes.h"
#include "SourceData.h"
#include "generator/Generator.h"

struct FLevelSettings;
class FOutputDevice;

namespace levelgen {

struct LevelGenData {
	LevelGenData(sourcedata::SourceData, generator::GeneratorFunc);

	sourcedata::SourceData sourceData;
	generator::GeneratorFunc generatorFunc;

	Runner& runner() const&;
private:
	mutable Unique<Runner> mRunner;
};

TOptional<LevelGenData> createLevelGenData(const FLevelSettings&, FOutputDevice* = nullptr);
TOptional<LevelGenData> createLevelGenData(Unique<sourcedata::SourceData>, const FLevelSettings&, FOutputDevice* = nullptr);

}
