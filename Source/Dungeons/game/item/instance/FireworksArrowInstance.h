#pragma once

#include "CoreMinimal.h"
#include "ArrowCraftingItemInstance.h"
#include "FireworksArrowInstance.generated.h"

class UItemSlot;

UCLASS()
class DUNGEONS_API AFireworksArrowInstance : public AArrowCraftingItemInstance
{
	GENERATED_BODY()
public:
	AFireworksArrowInstance();
	float GetStats(EItemStats stat) const override;
};
