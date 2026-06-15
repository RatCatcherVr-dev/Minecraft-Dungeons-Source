#pragma once

#include "CoreMinimal.h"
#include "ArrowCraftingItemInstance.h"
#include "HeavyHarpoonQuiver.generated.h"

UCLASS()
class DUNGEONS_API AHeavyHarpoonQuiver : public AArrowCraftingItemInstance
{
	GENERATED_BODY()
public:
	AHeavyHarpoonQuiver();
	float GetStats(EItemStats stat) const override;
};