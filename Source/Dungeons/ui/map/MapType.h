#pragma once

#include "common_header.h"

UENUM(BlueprintType)
enum class EMapType : uint8 {
	UNSET,
	Fullscreen,
	Overlay,	
	Max UMETA(Hidden),
};
ENUM_NAME(EMapType);

UENUM(BlueprintType)
enum class EOutsideBoundBehavior : uint8 {
	RevealExplored = 0,
	RevealAlways,
	Max UMETA(Hidden),
};
ENUM_NAME(EOutsideBoundBehavior);