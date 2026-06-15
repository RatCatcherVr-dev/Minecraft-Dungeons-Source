#pragma once

#include "common_header.h"

UENUM(BlueprintType)
enum class EEmote : uint8 {
	INVALID,
	ComeHere,
	Thanks,
	Ok,
	NeedArrows,
	Wait,
	NeedHealth,
	No,
	SuppliesHere,
	COUNT,
};
ENUM_NAME(EEmote);