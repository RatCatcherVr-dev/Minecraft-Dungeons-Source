#pragma once

#include "CoreMinimal.h"
#include "ArrowCraftingItemInstance.h"
#include "VoidQuiver.generated.h"

UCLASS()
class DUNGEONS_API AVoidQuiver : public AArrowCraftingItemInstance
{
	GENERATED_BODY()
public:
	AVoidQuiver();

	float GetStats(EItemStats stat) const override;

};
