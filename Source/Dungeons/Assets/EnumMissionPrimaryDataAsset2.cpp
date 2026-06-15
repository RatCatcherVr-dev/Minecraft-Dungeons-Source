#include "EnumMissionPrimaryDataAsset2.h"
#include "game/mission/MissionDef.h"
#include "game/mission/MissionDefs.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "ui/PostGameWidget.h"
#include "util/StringUtil.h"

ELevelNames UEnumMissionPrimaryDataAsset2::GetLevelName() const
{
	return level;
}

ELoadingScreenType UEnumMissionPrimaryDataAsset2::GetLoadingScreenType() const
{
	return ELoadingScreenType::Level;
}

bool UEnumMissionPrimaryDataAsset2::GetShouldPackageAssets() const {
	return missions::shouldPackageAssets(level);
}

TArray<FString> UEnumMissionPrimaryDataAsset2::GetFilenames() const {
	const auto& mission = missions::get(level);

	if (mission.isHyperMission()) {
		if (const auto hyperLevel = levelgen::sourcedata::hyperLevelFromFileSystem(mission.levelFilename())) {
			TArray<FString> out;
			algo::map_to(hyperLevel->levelIds, RETLAMBDA(stringutil::toFString((it.id))), out);
			algo::map_to(hyperLevel->definitionLevelIds, RETLAMBDA(stringutil::toFString((it.id))), out);
			return out;
		}
	}
	return { mission.levelFilename() };
}
