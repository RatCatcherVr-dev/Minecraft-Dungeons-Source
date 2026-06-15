#pragma once
#include <CoreMinimal.h>
#include <SoftObjectPtr.h>
#include <Map.h>
#include "game/difficulty/ExtraChallenge.h"
#include "util/Algo.hpp"
#include "ExtraChallengeStyle.generated.h"

USTRUCT(BlueprintType)
struct FExtraChallengeStyle {
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EExtraChallenge, TSoftObjectPtr<UTexture2D>> wings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EExtraChallenge, TSoftObjectPtr<UTexture2D>> icons;

	void appendTexturePaths(TArray<TSoftObjectPtr<UTexture2D>>& paths) const {
		paths.Append(algo::map_tarray(wings, RETLAMBDA(it.Value)));
		paths.Append(algo::map_tarray(icons, RETLAMBDA(it.Value)));
	};
};