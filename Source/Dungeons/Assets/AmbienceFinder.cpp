#include "AmbienceFinder.h"
#include "AssetFinder.h"
#include "game/level/ambience/AmbienceActor.h"
#include "util/Algo.h"
#include "util/EnumUtil.h"
#include <BlueprintSupport.h>
#include <Engine/AssetManager.h>
#include <Engine/Blueprint.h>

void UAmbienceFinder::Initialize(bool force) {
	if(bInitialized && !force) return;
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();


	//Copied from UObjectLibrary::LoadBlueprintAssetDataFromPaths source
#if WITH_EDITOR
	if (GIsEditor && !IsRunningCommandlet())
	{
		// The calls into AssetRegistery require /Game/ instead of /Game.
		// The calls further below, when setting up the ARFilters, do not want the trailing /.
		// (note: this is only an annoying edge case with /Game. Subfolders will work in both cases without the trailing /".
		if (AmbienceRoot.EndsWith(TEXT("/")) == false)
		{
			AmbienceRoot += TEXT("/");
		}
		
		AssetRegistry.ScanPathsSynchronous({AmbienceRoot});
	}
#endif

	FARFilter ARFilter;
	ARFilter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
	ARFilter.PackagePaths.Add(FName(*AmbienceRoot));
	ARFilter.bRecursivePaths = true;
	ARFilter.bIncludeOnlyOnDiskAssets = false;

	TArray<FAssetData> assets;
	AssetRegistry.GetAssets(ARFilter, assets);


	for(const auto& data : assets) {
		UClass* nativeClass = FindObject<UClass>(nullptr, *data.GetTagValueRef<FString>(FBlueprintTags::NativeParentClassPath));
		if (nativeClass && nativeClass->IsChildOf<AAmbienceActor>()) {
			TOptional<EAmbienceAudioID> audioId;
			TOptional<EAmbienceID> visualId;
			
			if(nativeClass->IsChildOf<AAmbienceAudioActor>()) {
				audioId = GetEnumValueFromStringT<EAmbienceAudioID>(data.GetTagValueRef<FName>(GET_MEMBER_NAME_CHECKED(AAmbienceAudioActor, Id)).ToString());
			} else if (nativeClass->IsChildOf<AAmbienceVisualActor>()) {
				visualId = GetEnumValueFromStringT<EAmbienceID>(data.GetTagValueRef<FName>(GET_MEMBER_NAME_CHECKED(AAmbienceVisualActor, Id)).ToString());
			}

			const auto group = GroupForData(data);
			MappedAssets.FindOrAdd(*group).Add({data, audioId, visualId});
		}
	}
}


FString UAmbienceFinder::GroupForData(const FAssetData& data) const {
	return GroupForPath(data.PackageName.ToString());
}

//Group is "/Game/Decor/Generated/<group>.... in path
FString UAmbienceFinder::GroupForPath(const FString& path) const {
	FString localPath(*path.RightChop(AmbienceRoot.Len()));
	return localPath.Left(localPath.Find("/"));
}


TArray<FSoftObjectPath> UAmbienceFinder::GetAmbiencesForGroup(const FString& group) const {
	if(const auto* data = MappedAssets.Find(*group)) {
		return algo::map_tarray(*data, RETLAMBDA(UAssetFinder::GetPathForAsset(it.data)));
	}

	return {};
}

TOptional<FSoftObjectPath> UAmbienceFinder::GetAudioAmbienceForGroupWithId(const FString& groupName, EAmbienceAudioID id) const {
	if(const auto* group = MappedAssets.Find(*groupName)) {
		if(const auto* entry = group->FindByPredicate(RETLAMBDA(it.audioId == id))) {
			return UAssetFinder::GetPathForAsset(entry->data);
		}
	}

	return {};
}
TOptional<FSoftObjectPath> UAmbienceFinder::GetVisualAmbienceForGroupWithId(const FString& groupName, EAmbienceID id) const {
	if(const auto* group = MappedAssets.Find(*groupName)) {
		if(const auto* entry = group->FindByPredicate(RETLAMBDA(it.visualId == id))) {
			return UAssetFinder::GetPathForAsset(entry->data);
		}
	}

	return {};
}