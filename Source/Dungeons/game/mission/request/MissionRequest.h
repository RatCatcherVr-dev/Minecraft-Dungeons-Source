#pragma once
#include "game/mission/MissionSelection.h"
#include "game/mission/state/MissionState.h"
#include "MissionRequest.generated.h"

UENUM(BlueprintType)
enum class EMissionRequestType : uint8 {
	Start,
	Continue,
};
ENUM_NAME(EMissionRequestType);

USTRUCT(BlueprintType)
struct DUNGEONS_API FMissionRequest {
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EMissionRequestType requestType;

	UPROPERTY(BlueprintReadOnly)
	FMissionState state;

	const FMissionState& getMissionState() const;
};