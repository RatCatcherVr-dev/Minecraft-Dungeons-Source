#pragma once

#include "CoreMinimal.h"
#include "Assets/MissionPrimaryDataAssetBase2.h"
#include "EnumMissionPrimaryDataAsset2.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UEnumMissionPrimaryDataAsset2 : public UMissionPrimaryDataAssetBase2
{
	GENERATED_BODY()

	TArray<FString> GetFilenames() const override;
	ELevelNames GetLevelName() const override;
	ELoadingScreenType GetLoadingScreenType() const override;
	bool GetShouldPackageAssets() const override;
public:
	UPROPERTY(EditAnywhere)
	ELevelNames level = ELevelNames::Invalid;
};
