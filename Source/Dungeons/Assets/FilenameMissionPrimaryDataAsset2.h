#pragma once

#include "CoreMinimal.h"
#include "Assets/MissionPrimaryDataAssetBase2.h"
#include "FilenameMissionPrimaryDataAsset2.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UFilenameMissionPrimaryDataAsset2 : public UMissionPrimaryDataAssetBase2
{
	GENERATED_BODY()

	TArray<FString> GetFilenames() const override { return { Filename }; }
	ELevelNames GetLevelName() const override;
	ELoadingScreenType GetLoadingScreenType() const override;
public:

	UPROPERTY(EditDefaultsOnly)
	FString Filename;
};
