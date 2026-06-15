
#include "FilenameMissionPrimaryDataAsset2.h"
#include "util/EnumUtil.h"

enum class ELoadingScreenType : uint8;

ELevelNames UFilenameMissionPrimaryDataAsset2::GetLevelName() const {
	return EnumValueFromString(ELevelNames, Filename).Get(ELevelNames::Invalid);
}

ELoadingScreenType UFilenameMissionPrimaryDataAsset2::GetLoadingScreenType() const {
	// enumMissionPrimaryDataAsset:s are levels
	return EnumValueFromString(ELoadingScreenType, Filename).GetValue();
}
