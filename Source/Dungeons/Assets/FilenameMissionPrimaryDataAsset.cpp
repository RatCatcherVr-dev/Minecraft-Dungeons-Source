#include "FilenameMissionPrimaryDataAsset.h"
#include "DungeonsDefsMinimal.h"
#include "util/EnumUtil.h"

ELevelNames UFilenameMissionPrimaryDataAsset::GetLevelName() const
{
	return GetEnumValueFromStringT<ELevelNames>(GetFilename()).Get(ELevelNames::Invalid);
}

ELoadingScreenType UFilenameMissionPrimaryDataAsset::GetLoadingScreenType() const
{
	return GetEnumValueFromStringT<ELoadingScreenType>(GetFilename()).Get(ELoadingScreenType::Level);
}
