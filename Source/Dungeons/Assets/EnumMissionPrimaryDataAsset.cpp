#include "EnumMissionPrimaryDataAsset.h"
#include "game/mission/MissionDef.h"
#include "game/mission/MissionDefs.h"
#include "ui/PostGameWidget.h"
#include "util/StringUtil.h"

ELevelNames UEnumMissionPrimaryDataAsset::GetLevelName() const
{
	return level;
}

ELoadingScreenType UEnumMissionPrimaryDataAsset::GetLoadingScreenType() const
{
	return ELoadingScreenType::Level;
}

bool UEnumMissionPrimaryDataAsset::GetShouldPackageAssets() const {
	return missions::shouldPackageAssets(level);
}

FString UEnumMissionPrimaryDataAsset::GetFilename() const
{
	return missions::get(level).levelFilename();
}
