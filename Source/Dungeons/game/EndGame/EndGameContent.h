#pragma once

#include "util/EnumUtil.h"
#include "common_header.h"
#include "EndGameContent.generated.h"

UENUM(BlueprintType)
enum class EEndGameContentType : uint8 {
	Invalid
	, EndlessStruggle
	, HyperMission
	, EmergentDifficulty
	, EnchanterMerchant
	, EndPortal
	, ENUM_COUNT
};
ENUM_NAME(EEndGameContentType);