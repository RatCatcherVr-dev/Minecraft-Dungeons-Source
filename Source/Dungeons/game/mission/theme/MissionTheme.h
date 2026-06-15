#pragma once

#include "util/EnumUtil.h"
#include "common_header.h"

UENUM(BlueprintType)
enum class EMissionTheme : uint8 {
	Invalid,
	Story,
	Secret,
	Daily,
	Weekly,
	Seasonal,
	Spooky,
	Jungle,
	Winter,
	Mountain,
	Hypermission,
	Anniversary,
	Spooky2021,
	count,
};
ENUM_NAME(EMissionTheme);