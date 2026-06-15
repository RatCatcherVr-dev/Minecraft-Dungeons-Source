#include "Dungeons.h"
#include "SubLevelLoader.h"
#include "TileDecor.h"
#include "lovika/tile/TilePlacement.h"
#include <StringConv.h>
#include "SubLevelTileLoader.h"

namespace game { namespace sublevel {

ULevelStreamingDynamic* TileLoader::schedule(const FString& path, const TilePlacement& tilePlacement)  {
	const FString originalTileId(tilePlacement.tileIdHACK().c_str());
	const auto fullPath = path + originalTileId;

	FString packageFilename;
	if (!FPackageName::DoesPackageExist(fullPath, NULL, &packageFilename)) {
		return nullptr;
	}

	const auto transform = decor::placementToTransform(tilePlacement.placement(), tilePlacement.bounds().size());

	FString uniqueName = fullPath;
	uniqueName.AppendChar('_');
	uniqueName.AppendInt(tilePlacement.placement().position.hashCode());
	uniqueName.Append(TEXT("_SkipPackageVerification"));
	return Loader::schedule(fullPath, uniqueName, transform);
}

}}
