#pragma once

#include "CoreMinimal.h"
#include "game/item/ItemType.h"
#include "game/item/SerializableItemId.h"
#include "game/item/InventoryItemData.h"
#include "PickupStorableComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStorableItemPickedUp, const FItemId&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnWillPickupItem, AStorableItem*);

USTRUCT()
struct DUNGEONS_API FItemPickupData {
	GENERATED_BODY()

	FItemPickupData() {}
	FItemPickupData(const TPair<FItemId, int32>& p) : id(p.Key), count(p.Value) {}

	static TArray<FItemPickupData> FromMap(const TMap<FItemId, int32>&);

	UPROPERTY()
	FSerializableItemId id;
	UPROPERTY()
	int32 count;
};
UCLASS()
class DUNGEONS_API UPickupStorableComponent : public UActorComponent
{
GENERATED_BODY()
public:
	void BeginPlay() override;
	bool Store(class AStorableItem*);

	FOnStorableItemPickedUp OnStorableItemPickedUp;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStore(class AStorableItem* item);

	TOptional<FItemId> GetLastCollectedItemId();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendPickupCounts(const TArray<FItemPickupData>& Data);

	UFUNCTION(Client, Reliable)
	void UpdateLastItem(const FInventoryItemData& Data);

	class APlayerCharacter* GetPlayer() const;
	bool HandleStorage(class AStorableItem*) const;

	void OnItemStored(const FItemId&, int32 count);

	class ABasePlayerController* Controller = nullptr;

	void ReplicatePickupCounts();

	TMap<FItemId, int32> PickupCounters;
	FTimerHandle ReplicationTimerHandle;

	AStorableItem* LastItemCollected;
	TOptional<FItemId> LastItemId;
};