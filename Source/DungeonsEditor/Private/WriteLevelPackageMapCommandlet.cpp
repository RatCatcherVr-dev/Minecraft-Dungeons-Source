
#include "WriteLevelPackageMapCommandlet.h"
#include <Engine/AssetManager.h>
#include <Engine/DataAsset.h>
#include <Misc/Paths.h>
#include <Misc/ConfigCacheIni.h>
#include "Dungeons/Assets/MissionPrimaryDataAssetBase.h"


int32 UWriteLevelPackageMapCommandlet::Main(const FString& Params) {
	UE_LOG(DungeonsEditor, Warning, TEXT("UWriteLevelPackageMapCommandlet"));

	TSet<FSoftObjectPath> NewPaths;

	UAssetManager& Manager = UAssetManager::Get();
	IAssetRegistry& AssetRegistry = Manager.GetAssetRegistry();

	FARFilter filter;

	// Assuming that all relevant MissionPrimaryDataAssetFiles are in this dir.
	filter.PackagePaths = { "/Game/Packaging" };
	filter.ClassNames = { UMissionPrimaryDataAssetBase::StaticClass()->GetFName() };
	filter.bRecursiveClasses = true;
	filter.bRecursivePaths = true;


	TArray<FAssetData> assetData;
	AssetRegistry.GetAssets(filter, assetData);

	UE_LOG(DungeonsEditor, Log, TEXT("- All level asset mappings -"));
	TArray<TTuple<FString,FString,FString>> UpdateRules;

	for (const auto& asset : assetData) {
		if (asset.IsValid()) {
			// log + print to file
			UE_LOG(DungeonsEditor, Log, TEXT(". %s <-> %d "), *asset.AssetName.ToString(), TEXT("NA"));
			asset.PrintAssetData();

			FPrimaryAssetId assetId = asset.GetPrimaryAssetId();

			// this is the Chunk, the ASSET ITSELF belongs to!
			//int32 id = asset.ChunkIDs.Num() > 0 ? asset.ChunkIDs[0] : -1;
			FString chunkIdTag;
			asset.GetTagValue("ChunkIDTag", chunkIdTag);
			FString label = "";
			FString overridePaths = "";

			asset.GetTagValue("PakRuleLabel", label);
			asset.GetTagValue("OverrideDataPakPaths", overridePaths);

			UE_LOG(DungeonsEditor, Log, TEXT("ChunkID on ASSET: %s"), *chunkIdTag);
			if (!chunkIdTag.IsEmpty() && !label.IsEmpty()) {
				UpdateRules.Add(TTuple<FString, FString, FString>(overridePaths, chunkIdTag, label));
			}
		}
	}

	// Write special rules into the DefaultPakFileRules for the cooker:
	if (GConfig) {
		FString DefaulPakFileRules = FPaths::ProjectDir() / "Config" / "DefaultPakFileRules.ini";

		int untaggedRulesCount = 0;

		for (auto rule : UpdateRules) {
			TArray<FStringFormatArg> LabelArg = { rule.Get<2>() };
			FString BuildPakSection = FString::Format(TEXT("Package{0}"), LabelArg);

			TArray<FString> taggingPaths;
			FString overridePaths = rule.Get<0>();
			TArray<FString> files;

			// Splice path spec with the 'label' unless overridePaths, is defined, if so: split on ';' and insert:
			if (overridePaths.IsEmpty()) {
				files = {
					FString::Format(TEXT(".../Content/data/lovika/levels/{0}.json"), LabelArg)
					,FString::Format(TEXT(".../Content/data/lovika/levels/{0}/*.json"), LabelArg)
					,FString::Format(TEXT(".../Content/data/lovika/objectgroups/{0}/*.json"), LabelArg)
					,FString::Format(TEXT(".../Content/data/resourcepacks/{0}/**/*"), LabelArg)
				};
			}
			else {
				overridePaths.ParseIntoArray(taggingPaths, TEXT(";"), true);
				for (const FString& p : taggingPaths) {
					files.Add(TEXT(".../Content/data/") + p.TrimStartAndEnd());
				}

			}

			TArray<FStringFormatArg> chunk_values;
			chunk_values.Add(rule.Get<1>());
			FString pkchunk = FString::Format(TEXT("pakchunk{0}"), chunk_values);

			GConfig->SetString(
				*BuildPakSection,
				TEXT("OverridePaks"),
				*pkchunk,
				DefaulPakFileRules
			);
			GConfig->SetArray(*BuildPakSection, TEXT("+Files"), files, DefaulPakFileRules);
			GConfig->SetBool(*BuildPakSection, TEXT("bExcludeFromPaks"), false, DefaulPakFileRules);

		}
		GConfig->Flush(false, DefaulPakFileRules);
	}
	UE_LOG(DungeonsEditor, Log, TEXT("finished, AssetManager oK"));

	return 0;
}
