#pragma once

#include "Dungeons.h"

UENUM(BlueprintType)
enum class ETeamRelation : uint8 {
	Hostile,
	Neutral,
	Friendly,
};
ENUM_NAME(ETeamRelation);