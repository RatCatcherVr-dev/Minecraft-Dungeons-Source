

#pragma once

#include "CoreMinimal.h"
#include "Assets/MissionPrimaryDataAssetBase.h"
#include "FilenameMissionPrimaryDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UFilenameMissionPrimaryDataAsset : public UMissionPrimaryDataAssetBase
{
	GENERATED_BODY()

	FString GetFilename() const override { return Filename; }
	ELevelNames GetLevelName() const override;
	ELoadingScreenType GetLoadingScreenType() const override;
public:

	UPROPERTY(EditDefaultsOnly)
	FString Filename;
};
