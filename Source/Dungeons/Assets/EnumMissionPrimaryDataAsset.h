

#pragma once

#include "CoreMinimal.h"
#include "Assets/MissionPrimaryDataAssetBase.h"
#include "EnumMissionPrimaryDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UEnumMissionPrimaryDataAsset : public UMissionPrimaryDataAssetBase
{
	GENERATED_BODY()

	FString GetFilename() const override;
	ELevelNames GetLevelName() const override;
	ELoadingScreenType GetLoadingScreenType() const override;
	bool GetShouldPackageAssets() const override;
public:
	UPROPERTY(EditAnywhere)
	ELevelNames level = ELevelNames::Invalid;
};
