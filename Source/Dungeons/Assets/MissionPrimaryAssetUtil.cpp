#include "Dungeons.h"
#include "MissionPrimaryAssetUtil.h"
#include "game/GameTypes.h"
#include "game/level/TileDecor.h"
#include "game/level/doors/DoorUtil.h"
#include "game/objective/ChallengeSystem.h"
#include "LoadingScreen/LoadingScreenBuilder.h"
#include "lovika/world/level/levelgen/SourceData.h"
#include "world/level/block/Block.h"
#include "util/StringUtil.h"

namespace missionasset {

TSet<int> GetDoorSizesForTiles(const MetaTileVector& tiles)
{
	TSet<int> sizes;

	for (const auto& tile : tiles)
	{
		for (const auto& door : tile.tile().doors())
		{
			sizes.Add(door.width());
		}
	}

	return sizes;
}

TSet<FSoftObjectPath> ExtractValidAssets(const TArray<FAssetData>& potentialAssets, const UAssetManager& Manager)
{
	TSet<FSoftObjectPath> paths;
	for (const auto& asset : potentialAssets)
	{
		FSoftObjectPath path = Manager.GetAssetPathForData(asset);
		if (path.IsValid()) paths.Add(path);
	}

	return paths;
}

TSet<FSoftObjectPath> ValidAssetsFromPackageNames(TSet<FName>&& packageNames, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	FARFilter filter;
	filter.PackageNames = std::move(packageNames.Array());

	TArray<FAssetData> assetData;
	AssetRegistry.GetAssets(filter, assetData);

	return ExtractValidAssets(assetData, Manager);
}

TSet<FName> GetIntroAsset(FString filename, const levelgen::sourcedata::SourceData& sourceData) {
	FString levelId = stringutil::toFString(sourceData.level.id);
	FString assetpath = filename;
	if (!levelId.Contains(filename)) { // allow 'Contains()', to nod add the assets for [levelname]daily.
		UE_LOG(LogDungeons, Warning, TEXT("levelId (%s) different from filename (%s), preferring levelId!"), *levelId, *filename);
		assetpath = levelId;
	}
	return { *decor::filenameForActor(assetpath, FString("intro")) };
}

TSet<FName> GetDoorPackageNamesForObjectAndSizes(const TSet<FString>& objects, const TSet<int>& doorSizes)
{
	TSet<FName> potentialDoors;
	for (const auto& doorPath : objects)
	{
		potentialDoors.Add(*doorPath);

		for (const auto& size : doorSizes)
		{
			potentialDoors.Add(*game::PrefabPath(doorPath, size));
		}
	}

	return potentialDoors;
}

TSet<FSoftObjectPath> GetDoorPathsForObjectAndSizes(const TSet<FString>& objects, const TSet<int>& doorSizes, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	return ValidAssetsFromPackageNames(GetDoorPackageNamesForObjectAndSizes(objects, doorSizes), AssetRegistry, Manager);
}

TSet<FSoftObjectPath> GetTravelDoorPrefabsForTiles(const MetaTileVector tiles, const TSet<int>& doorSizes, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	TSet<FString> PackagesToFind;

	//Find all potential door sizes and all teleports. Since we cannot know actual teleport door sizes at this stage.
	for (const auto& tile : tiles)
	{
		for (const auto& teleport : tile.metadata.teleports)
		{
			FString path = game::PrefabPath(teleport.object.Get(game::door::DEFAULT_TRAVEL_DOOR_PREFAB_PATH));
			PackagesToFind.Add(path);
		}
	}

	return GetDoorPathsForObjectAndSizes(PackagesToFind, doorSizes, AssetRegistry, Manager);
}

TSet<FSoftObjectPath> GetDoorPrefabsForTiles(const MetaTileVector& tiles, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	TSet<FString> doorsToLoad;

	for (const auto& tile : tiles)
	{
		for (const auto& door : tile.tile().doors())
		{
			doorsToLoad.Append(game::door::generatePreferredPrefabPaths(door));
		}
	}

	TSet<FName> packageNames;
	packageNames.Reserve(doorsToLoad.Num());
	for (const auto& path : doorsToLoad) packageNames.Add(*path);

	return ValidAssetsFromPackageNames(MoveTemp(packageNames), AssetRegistry, Manager);
}

TOptional<FString> GetGateForObjective(const io::Objective& objective)
{
	TOptional<FString> gatePath;
	switch (objective.type)
	{
	case io::ObjectiveType::None: break;
	case io::ObjectiveType::Arena:
		gatePath = objective.arena.GetValue().gate.object;
		break;
	case io::ObjectiveType::Click:
		gatePath = objective.click.GetValue().doorPath;
		break;
	case io::ObjectiveType::Gauntlet:
		gatePath = objective.gauntlet.GetValue().gate.object;
		break;
	case io::ObjectiveType::KillGroup:
		gatePath = objective.killGroup.GetValue().gate.object;
		break;
	}

	if (gatePath.IsSet())
		return game::PrefabPath(gatePath.GetValue());

	return {};
}

TSet<FName> GetObjectPathsForObjective(const io::Objective& objective)
{
	TSet<FName> Paths;

	switch (objective.type) {
	case io::ObjectiveType::None: break;
	case io::ObjectiveType::Arena:
		if (const auto& preSpawn = objective.arena.GetValue().preSpawnMobObject) Paths.Add(*stringutil::toFString(preSpawn.GetValue()));
		break;
	case io::ObjectiveType::Click:
		Paths.Add(*game::PrefabPath(stringutil::toFString(objective.click.GetValue().object)));
		if (const auto& dummy = objective.click.GetValue().dummyObject) Paths.Add(*game::PrefabPath(dummy.GetValue()));
		break;
	case io::ObjectiveType::Gauntlet: break;
	case io::ObjectiveType::KillGroup: break;
	}

	return Paths;
}

TSet<FSoftObjectPath> GetAllMatchingDoors(FString AssetNameBase, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	TSet<FSoftObjectPath> assets;
	FARFilter filter;

	FString AssetPath;
	FString AssetBase;
	AssetNameBase.Split(TEXT("/"), &AssetPath, &AssetBase, ESearchCase::CaseSensitive, ESearchDir::FromEnd);
	filter.PackagePaths = { FName(*(AssetPath)) };

	TArray<FAssetData> assetData;
	AssetRegistry.GetAssets(filter, assetData);

	for (FAssetData data : assetData) {
		if (data.AssetName.ToString().Contains(AssetBase))
			assets.Add(data.ToSoftObjectPath());
	}
	return assets;
}


TSet<FSoftObjectPath> GetPrefabsForObjective(const io::Objective& objective, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	TSet<FName> packageNames = GetObjectPathsForObjective(objective);
	TSet<FSoftObjectPath> out = ValidAssetsFromPackageNames(MoveTemp(packageNames), AssetRegistry, Manager);
	if (auto gate = GetGateForObjective(objective)) {
		if (gate.IsSet()) {
			out.Append(GetAllMatchingDoors(gate.GetValue(), AssetRegistry, Manager));
		}
	}

	return out;
}

TSet<FSoftObjectPath> GetPrefabsForObjectives(const std::vector<io::Objective>& objectives, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	TSet<FSoftObjectPath> Assets;

	for (const auto& objective : objectives)
	{
		Assets.Append(GetPrefabsForObjective(objective, AssetRegistry, Manager));
	}

	const auto& defaults = game::objective::ChallengeSystem::DEFAULT_REWARD_PREFABS;
	TSet<FName> outPaths;
	outPaths.Reserve(defaults.size());

	for (const auto& prefab : defaults) outPaths.Add(*game::PrefabPath(prefab));

	Assets.Append(ValidAssetsFromPackageNames(MoveTemp(outPaths), AssetRegistry, Manager));

	return Assets;
}

TSet<FSoftObjectPath> GetPrefabsForChallengesForTiles(const MetaTileVector& tiles, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	TSet<FSoftObjectPath> Paths;

	for (const auto& tile : tiles)
	{
		for (const auto& challenge : tile.metadata.challenges)
		{
			Paths.Append(GetPrefabsForObjective(challenge.objective, AssetRegistry, Manager));
			FARFilter packageFilter;
			if (challenge.triggerObject.IsSet()) packageFilter.PackageNames.Add(*game::PrefabPath(challenge.triggerObject.GetValue()));
			if (challenge.reward.customPrefab.IsSet()) packageFilter.PackageNames.Add(*game::PrefabPath(challenge.reward.customPrefab.GetValue()));

			if (packageFilter.PackageNames.Num())
			{
				TArray<FAssetData> assetData;
				AssetRegistry.GetAssets(packageFilter, assetData);
				Paths.Append(ExtractValidAssets(assetData, Manager));
			}
		}
	}

	return Paths;
}

TSet<FSoftObjectPath> GetSublevelsForTiles(const TArray<FString>& potentialPaths, IAssetRegistry& AssetRegistry, const UAssetManager& Manager, const MetaTileVector& tiles) {
	TArray<FAssetData> packageData;

	for (const auto& tile : tiles) {
		for (const auto& path : potentialPaths) {
			FString potentialLevel = path + FString(tile.tile().id().c_str());
			AssetRegistry.GetAssetsByPackageName(*potentialLevel, packageData, true);
		}
	}

	return ExtractValidAssets(packageData, Manager);
}

static TSharedPtr<game::loadingscreen::LoadingScreenBuilder> LSBuilder;

TSet<FSoftObjectPath> GetLoadingscreenAssets(ELoadingScreenType loadingscreenType, ELevelNames levelName) {
	TSet<FSoftObjectPath> assets;
	if (!LSBuilder)
		LSBuilder = MakeShareable<game::loadingscreen::LoadingScreenBuilder>(new game::loadingscreen::LoadingScreenBuilder());

	assets.Add(LSBuilder->GetTexture(loadingscreenType, levelName));
	return assets;
}

//// mission loadingscreen-assets
TSet<FSoftObjectPath> GetEnvironmentalAssetsForMission(const FString& missionId, IAssetRegistry& AssetRegistry, const UAssetManager& Manager)
{
	TArray<FAssetData> DirectoryAssets;
	FString path = decor::filenameForEnvironmental(missionId, "");
	if (path.EndsWith("/")) path.RemoveAt(path.Len() - 1);

	AssetRegistry.GetAssetsByPath(*path, DirectoryAssets, true);

	return ExtractValidAssets(DirectoryAssets, Manager);
}

bool IsBuildSystemInitialized() {
	return Block::mFillGradient != nullptr;
}

}
