#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "game/item/SerializableItemId.h"
#include "ItemDropActor.generated.h"

/**
 * World global actor that provides on demand item drops of a specific type.
 */
UCLASS()
class DUNGEONS_API AItemDropActor : public AInfo
{
	GENERATED_BODY()
	
public:	
	AItemDropActor();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void DropItems(const FVector& spawnLocation, const FSerializableItemId& itemType, const int32 dropCount, AActor* targetPlayer, bool lockItemToOwner = false) const;
};
