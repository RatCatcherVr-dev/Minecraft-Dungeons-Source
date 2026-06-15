#pragma once
#include "UObject/ObjectMacros.h"
#include "util/EnumUtil.h"

UENUM(BlueprintType)
enum class ELevelVariationType : uint8 {
	Invalid,

	none UMETA(DisplayName = "Default"), //D11.PS renamed default to none
	daily UMETA(DisplayName = "Daily"),
	weekly UMETA(DisplayName = "Weekly"),
	seasonal UMETA(DisplayName = "Seasonal")
};
ENUM_NAME(ELevelVariationType);