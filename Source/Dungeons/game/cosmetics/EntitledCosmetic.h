#pragma once

#include "CoreMinimal.h"
#include "CosmeticItemDef.h"
#include "EntitledCosmetic.generated.h"

USTRUCT()
struct DUNGEONS_API FEntitledCosmetic {
	
	GENERATED_BODY()

	FEntitledCosmetic() = default;
	explicit FEntitledCosmetic(FName cosmeticId, FEntitlement entitlement);

	UPROPERTY()
	FName CosmeticId;

	UPROPERTY()
	FEntitlement Entitlement;
}; 