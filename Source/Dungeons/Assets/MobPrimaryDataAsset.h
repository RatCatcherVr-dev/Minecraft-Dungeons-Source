#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include <Engine/AssetManagerTypes.h>
#include "MobPrimaryDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class DUNGEONS_API UMobPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

#if WITH_EDITORONLY_DATA
		virtual void UpdateAssetBundleData();
#endif
public:
	UMobPrimaryDataAsset();

	static const FName MobsBundle;

	/** Set to true if the label asset itself should be cooked and available at runtime. This does not affect the assets that are labeled, they are set with cook rule */
	UPROPERTY(EditAnywhere, Category = PrimaryAssetLabel)
	uint32 bIsRuntimeLabel : 1;

	/** Management rules for this specific asset, if set it will override the type rules */
	UPROPERTY(EditAnywhere, Category = Rules, meta = (ShowOnlyInnerProperties))
	FPrimaryAssetRules Rules;

	UPROPERTY(EditAnywhere, Category = PrimaryAssetLabel)
	bool ForceLoadAll = true;

	/** Set to editor only if this is not available in a cooked build */
	bool IsEditorOnly() const override
	{
		return !bIsRuntimeLabel;
	}

};
