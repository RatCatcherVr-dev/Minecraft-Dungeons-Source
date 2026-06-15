#pragma once
#include <CoreMinimal.h>
#include <SoftObjectPtr.h>
#include "MissionMarkerStyle.generated.h"

USTRUCT(BlueprintType)
struct FMissionMarkerStyle {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Backplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D>	Background;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Shadow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> HighlightOutline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Border;

	void appendIfValid(const TSoftObjectPtr<UTexture2D>& path, TArray<TSoftObjectPtr<UTexture2D>>& paths) const {
		if (path.IsValid()) {
			paths.Add(path);
		}
	}
	void appendTexturePaths(TArray<TSoftObjectPtr<UTexture2D>>& paths) const {
		appendIfValid(Backplate, paths);
		appendIfValid(Background, paths);
		appendIfValid(Shadow, paths);
		appendIfValid(HighlightOutline, paths);
		appendIfValid(Border, paths);
	};
};