#include "AssetUtil.h"

FString MakeAssetPathFromPackageName(const FString& path)
{
	int index = INDEX_NONE;
	if (path.FindLastChar('.', index)) return path;

	FString assetName = path;

	if (path.FindLastChar('/', index))
	{
		assetName = path.RightChop(index + 1);
	}

	return path + "." + assetName + "_C";
}


FString MakeBlueprintPathFromPackageName(const FString& path)
{
	FString assetPath = MakeAssetPathFromPackageName(path);

	return assetPath.EndsWith("_C") ? assetPath : (assetPath + "_C");
}
