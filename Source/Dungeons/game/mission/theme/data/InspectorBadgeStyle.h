#pragma once
#include <CoreMinimal.h>
#include "ExtraChallengeStyle.h"
#include "InspectorBadgeStyle.generated.h"

USTRUCT(BlueprintType)
struct FInspectorBadgeStyle {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Badge;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FExtraChallengeStyle ExtraChallenge;

	void appendTexturePaths(TArray<TSoftObjectPtr<UTexture2D>>& paths) const {
		if(Badge.IsValid()){
			paths.Add(Badge);
		}
		ExtraChallenge.appendTexturePaths(paths);
	};
};