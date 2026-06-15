#pragma once

#include "CoreMinimal.h"
#include "ArrowCraftingItemInstance.h"
#include "FlamingQuiver.generated.h"

UCLASS()
class DUNGEONS_API AFlamingQuiver : public AArrowCraftingItemInstance
{
	GENERATED_BODY()
public:
	AFlamingQuiver();
	float GetStats(EItemStats stat) const override;
};
