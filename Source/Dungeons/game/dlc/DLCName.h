#pragma once

#include "util/EnumUtil.h"
#include "common_header.h"

UENUM(BlueprintType)
enum class EDLCName : uint8 {
	Invalid,
	TheJungleAwakens,
	TheCreepingWinter,
	Mountains,
	Nether,
	Oceans,
	TheEnd,
	count
};
ENUM_NAME(EDLCName);