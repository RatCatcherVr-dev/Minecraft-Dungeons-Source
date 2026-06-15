#include "Dungeons.h"
#include "PickupStorableComponent.h"
#include "PickupItemComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/item/StorableItem.h"
#include "game/item/ItemUtil.h"
#include "DungeonsGameInstance.h"
#include "game/util/DungeonsGearUtilLibrary.h"

TArray<FItemPickupData> FItemPickupData::FromMap(const TMap<FItemId, int32>& map) {
	TArray<FItemPickupData> data;
	for (const auto& entry : map) {
		data.Emplace(entry);
	}

	return data;
}

void UPickupStorableComponent::BeginPlay() {
	Super::BeginPlay();
	Controller = Cast<ABasePlayerController>(GetOwner());
}

void UPickupStorableComponent::ServerSendPickupCounts_Implementation(const TArray<FItemPickupData>& Data) {
	for (const auto& d : Data) {
		OnItemStored(d.id, d.count);
	}
}

bool UPickupStorableComponent::ServerSendPickupCounts_Validate(const TArray<FItemPickupData>& Data) {
	return true;
}

void UPickupStorableComponent::ServerStore_Implementation(class AStorableItem* item) {
	if (!item) return; //Item can have been destroyed here....
	if (Store(item)) {
		UpdateLastItem(item->ItemData);
	};
}

bool UPickupStorableComponent::ServerStore_Validate(class AStorableItem* item) {
	return true;
}


// Broadcast from here to UIHintActor etc.
bool UPickupStorableComponent::Store(AStorableItem* item) {
	if (!item->stored && HandleStorage(item)) {
		OnItemStored(item->GetItemType().getId(), item->ItemData.OverrideStoreCount.Get(1));
		item->stored = true;
		item->OnPickup();

		OnStorableItemPickedUp.Broadcast(item->ItemData.GetItemId());
		LastItemId = GetItemRegistry().Get(item->ItemData.GetItemId()).getId();

		item->DestroyOrReturnToPool();
		return true;
	}
	item->OnPickupDenied();
	return false;
}

TOptional<FItemId> UPickupStorableComponent::GetLastCollectedItemId()
{
	return LastItemId;
}

bool UPickupStorableComponent::HandleStorage(AStorableItem* item) const {
	if (!item->HasAuthority() && !game::item::util::ItemActorIsLocalOnly(item)) {
		UE_LOG(LogTemp, Warning, TEXT("Trying to store server owned item - Ignoring"));
		return false;
	}

	auto player = GetPlayer();
	if (!player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to store item on non-existing player - Ignoring"));
		return false;
	}
	return player->GetPickupItemComponent()->Pickup(item->ItemData);
}

void UPickupStorableComponent::OnItemStored(const FItemId& id, int32 count) {
	//Hacky filter...for now.
	if (id != game::item::type::Emerald.getId()) return;

	if (GetOwnerRole() == ROLE_Authority) {

		DungeonsGearUtilLibrary::OnEmeraldsCollected(GetPlayer(), count);
	}
	else {
		if (!PickupCounters.Contains(id)) {
			PickupCounters.Add(id) = 0;
		}

		PickupCounters[id] += count;

		if (!ReplicationTimerHandle.IsValid()) {
			ReplicationTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UPickupStorableComponent::ReplicatePickupCounts));
		}
	}
}

void UPickupStorableComponent::ReplicatePickupCounts() {
	ServerSendPickupCounts(FItemPickupData::FromMap(PickupCounters));
	PickupCounters.Empty();
	ReplicationTimerHandle.Invalidate();
}

void UPickupStorableComponent::UpdateLastItem_Implementation(const FInventoryItemData& Data)
{
	OnStorableItemPickedUp.Broadcast(Data.GetItemId());
	LastItemId = GetItemRegistry().Get(Data.GetItemId()).getId();
}

APlayerCharacter* UPickupStorableComponent::GetPlayer() const {
	return Controller->GetControlledPlayerCharacter();
}