#pragma once

#include "CoreMinimal.h"
#include "ArrowCraftingItemInstance.h"
#include "ThunderingQuiver.generated.h"

UCLASS()
class DUNGEONS_API AThunderingQuiver : public AArrowCraftingItemInstance
{
	GENERATED_BODY()
public:
	AThunderingQuiver();
	float GetStats(EItemStats stat) const override;
};
