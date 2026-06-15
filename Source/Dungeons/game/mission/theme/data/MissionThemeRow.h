#pragma once
#include "Engine/DataTable.h"
#include "MissionThemeMarkerStyle.h"
#include "InspectorBadgeStyle.h"
#include "MissionNewsStyle.h"
#include "LoadingScreenStyle.h"
#include "MissionThemeRow.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionThemeRow : public FTableRowBase {
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FMissionThemeMarkerStyle Marker;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FInspectorBadgeStyle Badge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FMissionNewsStyle News;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FLoadingScreenStyle LoadingScreen;

	TArray<TSoftObjectPtr<UTexture2D>> getSoftTexturePtrs() const;	
};
