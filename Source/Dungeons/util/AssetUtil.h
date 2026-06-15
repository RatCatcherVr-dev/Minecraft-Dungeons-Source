#pragma once
#include "UnrealString.h"


/**
 * \brief f Makes a path/to/asset path conform to the unreal path/to/asset.asset
 * \param path 
 * \return 
 */
FString MakeAssetPathFromPackageName(const FString& path);
/**
 * \brief Makes a path/to/bp path conform to the unreal path/to/bp.bp_C
 * \param path 
 * \return 
 */
FString MakeBlueprintPathFromPackageName(const FString& path);