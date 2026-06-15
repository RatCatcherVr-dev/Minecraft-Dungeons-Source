#include "Dungeons.h"
#include "SubLevelUtil.h"
#include <Engine/LevelStreamingDynamic.h>
#include <Engine.h>

namespace game { namespace sublevel {

ULevelStreamingDynamic* createUnloaded(UWorld& world, const FString& path, const FString& uniqueName) {
	FString packageFileName;
	if (!FPackageName::DoesPackageExist(path, NULL, &packageFileName)) {
		UE_LOG(LogTemp, Warning, TEXT("Trying to load invalid level %s"), *path);
		return nullptr;
	}

	ULevelStreamingDynamic* level = NewObject<ULevelStreamingDynamic>(&world, NAME_None, RF_NoFlags, NULL);

	// Associate a package name.
	level->SetWorldAssetByPackageName(FName(*uniqueName));
	if (world.IsPlayInEditor()) {
		FWorldContext WorldContext = GEngine->GetWorldContextFromWorldChecked(&world);
		level->RenameForPIE(WorldContext.PIEInstance);
	}

	level->LevelColor = FColor::MakeRandomColor();
	level->bInitiallyLoaded = true;
	level->bInitiallyVisible = true;
	level->bShouldBlockOnLoad = false;
	level->SetShouldBeLoaded(level->bInitiallyLoaded);
	level->SetShouldBeVisible(level->bInitiallyVisible);
	level->PackageNameToLoad = FName(*FPackageName::FilenameToLongPackageName(packageFileName));
	return level;
}

ULevelStreamingDynamic* load(UWorld& world, const FString& path, const FString& uniqueName, const FTransform& transform /* = {}*/) {
	if (auto level = createUnloaded(world, path, uniqueName)) {
		level->LevelTransform = transform;
		world.AddStreamingLevel(level);
		return level;
	}
	return nullptr;
}

}}
