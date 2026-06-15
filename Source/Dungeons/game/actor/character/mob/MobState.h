#pragma once

#include "MobState.generated.h"

UENUM(BlueprintType)
enum class EMobState2 : uint8 {
	Idle,
	Combat,
	Staggered,
	Fleeing
};
