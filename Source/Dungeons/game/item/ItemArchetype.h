#pragma once

#include "UObject/ObjectMacros.h"
#include "common_header.h"

UENUM(BlueprintType)
enum class EItemArchetype : uint8 {
	Acrobat,
	Archer,
	Fighter,
	Mage,
	Soul,
	Summoner,
	Support,
	Tank,
	Special
};
ENUM_NAME(EItemArchetype);
