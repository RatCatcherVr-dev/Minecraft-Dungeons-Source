#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "game/levels.h"
#include <Engine/AssetManagerTypes.h>
#include "DungeonsDefsMinimal.h"
#include "MissionPrimaryDataAssetBase.generated.h"


UCLASS(Abstract)
class DUNGEONS_API UMissionPrimaryDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	virtual FString GetFilename() const { return FString(); };
protected:
	virtual ELevelNames GetLevelName() const { return ELevelNames::Invalid; }
	virtual ELoadingScreenType GetLoadingScreenType() const { return ELoadingScreenType::Count; }

	virtual bool GetShouldPackageAssets() const { return true; };

	FPrimaryAssetId GetPrimaryAssetId() const override;
public:
	static const FName BundleName;
	
	UMissionPrimaryDataAssetBase();

	/** Set to true if the label asset itself should be cooked and available at runtime. This does not affect the assets that are labeled, they are set with cook rule */
	UPROPERTY(EditAnywhere, Category = PrimaryAssetLabel)
	uint32 bIsRuntimeLabel : 1;

	/** Management rules for this specific asset, if set it will override the type rules */
	UPROPERTY(EditAnywhere, Category = Rules, meta = (ShowOnlyInnerProperties))
	FPrimaryAssetRules Rules;

	/** expose ChunkId for tagging 'loose' files (in data/lovika/*) for chunking */
	UPROPERTY(BlueprintReadOnly, Category = PakDungeonsFiles, AssetRegistrySearchable)
	int32 ChunkIDTag;

	/** Label to (Default)PakFileRules, if this is non-empty, will tag files in '.../Content/data/resourcepacks|level]/{}' for this Primary Asset's Rules.ChunkID */
	UPROPERTY(EditAnywhere, Category = PakDungeonsFiles, BlueprintReadOnly, AssetRegistrySearchable)
	FString PakRuleLabel;

	/** Semicolon-separated set of wildcard-paths, relative to '.../Content/data/ to tag assets in that path for this Mission/Chunk. This will override the default set of paths added w r t setting PakRuleLabel*/
	UPROPERTY(EditAnywhere, Category = PakDungeonsFiles, BlueprintReadOnly, AssetRegistrySearchable)
	FString OverrideDataPakPaths;

	/** Set to editor only if this is not available in a cooked build */
	bool IsEditorOnly() const override
	{
		return !bIsRuntimeLabel;
	}
	

#if WITH_EDITORONLY_DATA
	void UpdateAssetBundleData() override;
#endif
};
