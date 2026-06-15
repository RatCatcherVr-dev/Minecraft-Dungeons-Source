#pragma once
#include "UObject/ObjectMacros.h"
#include "util/EnumUtil.h"

UENUM(BlueprintType)
enum class EMissionChancesType : uint8 {	
	Gear,
	Artifacts,
	Rewards,
	Ancients,
};
ENUM_NAME(EMissionChancesType);
