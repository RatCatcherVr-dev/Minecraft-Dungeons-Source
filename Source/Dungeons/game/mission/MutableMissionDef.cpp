#include "Dungeons.h"
#include "MutableMissionDef.h"
#include "lovika/world/level/levelgen/SourceData.h"

namespace missions {

MutableMissionDef& MutableMissionDef::hyperMission() {
	mIsHyperMission = true;

	mHyperDungeons = LazyHyperDungeons([this](std::vector<io::HyperDungeon>& outDungeons) {
		if (auto hyperLevel = levelgen::sourcedata::hyperLevelFromFileSystem(levelFilename())) {
			outDungeons = hyperLevel->hyperDungeons;
		}
	});
	return *this;
}

missions::MutableMissionDef& MutableMissionDef::appendAffectors( affector::RuleData data ) {
	for( auto& x : data.Data ) {
		mAffectors.Set(x.Key, x.Value);
	}
	return *this;
}

}
