#pragma once

#include "util/EnumUtil.h"

UENUM(BlueprintType)
enum class ERealmName : uint8 {
	Invalid,
	ArchIllagerRealm UMETA(DisplayName = "Arch Illager Realm"),
	IslandsRealm UMETA(DisplayName = "Islands Realm"),
	OtherDimensions UMETA(DisplayName = "Other Dimensions"),
	count
};
ENUM_NAME(ERealmName);