#pragma once
#include "OfferingsSummary.generated.h"

USTRUCT()
struct DUNGEONS_API FOfferingsSummary {
	GENERATED_BODY()

	UPROPERTY()
	int offeredCount = 0;

	UPROPERTY()
	int offeredTotalPower = 0;

	UPROPERTY()
	int offeredEnchantmentPoints = 0;
};
