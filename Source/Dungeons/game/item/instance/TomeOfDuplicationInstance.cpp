#include "TomeOfDuplicationInstance.h"
#include "game/ArmorProperties/ItemCooldownDecrease.h"
#include "../ItemUtil.h"
#include "../power/ItemPowerEffectDefs.h"
#include "game/component/PickupStorableComponent.h"
#include "game/actor/item/ItemEmitterActor.h"

ATomeOfDuplicationInstance::ATomeOfDuplicationInstance() {
	bCanFail = false;
	PowerEffects = { UDoubleItemChanceIncrease::StaticClass() };
	SharedPassiveCooldown = 0.f;
}

void ATomeOfDuplicationInstance::Activate(const FPredictionKey& predictionKey) {
	Super::Activate(predictionKey);

	if (GetOwner()->GetInstigatorController()->IsLocalController() && LastItemCollected.IsSet()) {
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [=]() {
			ServerDropItem(LastItemSerializableItem);
			if (FMath::FRand() < GetPowerEffect()->GetMultiplier(ItemPower)) {
				ServerDropItem(LastItemSerializableItem);
			}
		}, 1.0f, false);
	}
}

void ATomeOfDuplicationInstance::OnSetupWithValidOwner()
{
	Super::OnSetupWithValidOwner();

	if (GetOwner()->GetInstigatorController() && GetOwner()->GetInstigatorController()) {
		if (UPickupStorableComponent* PickUpComponent = GetOwner()->GetInstigatorController()->FindComponentByClass<UPickupStorableComponent>()) {
			if (PickUpComponent->GetLastCollectedItemId().IsSet()) {
				OnItemCollected(PickUpComponent->GetLastCollectedItemId().GetValue());
			}
			PickUpComponent->OnStorableItemPickedUp.AddUObject(this, &ATomeOfDuplicationInstance::OnItemCollected);
		}
	}
}

bool ATomeOfDuplicationInstance::CanActivate() const
{
	return Super::CanActivate() && LastItemCollected.IsSet();
}

bool ATomeOfDuplicationInstance::ServerDropItem_Validate(FSerializableItemId itemToSpawn)
{
	return itemToSpawn.IsValid();
}

void ATomeOfDuplicationInstance::ServerDropItem_Implementation(FSerializableItemId itemToSpawn)
{
	FInventoryItemData inventoryItemData = FInventoryItemData(GetItemRegistry().Get(itemToSpawn).getId());
		AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), { inventoryItemData }, GetOwner()->GetActorLocation(), GetOwner(), false, false, false, FItemEmitterActorCompleteDelegate::CreateLambda([=](TArray< AStorableItem* >& SpawnedItems) {

			for (auto item : SpawnedItems)
			{
				item->SetReplicates(true);
				item->ApplyDropEffect(GetOwner(), false);
				item->SetTimeToPool(StorableLifeSpan);
			}

		}));
}

const FSerializableItemId& ATomeOfDuplicationInstance::GetAlternativeItemId() const
{
	return LastItemCollected.IsSet() ? LastItemSerializableItem : Super::GetItemId();
}

void ATomeOfDuplicationInstance::OnItemCollected(const FItemId& ItemCollected)
{
	if (CanItemIdBeConsidered(ItemCollected)) {
		LastItemCollected = ItemCollected;
		LastItemSerializableItem = FSerializableItemId(ItemCollected);
		OnAlternativeItemChanged.Broadcast();
	}
}

bool ATomeOfDuplicationInstance::CanItemIdBeConsidered(const FItemId& ItemToCheck)
{
	const ItemType& itemType = GetItemRegistry().Get(ItemToCheck);
	ItemTag itemCollectedTag = itemType.getTag();
	ESlotType itemSlotType = itemType.slotType();
	return (!LastItemCollected.IsSet() || itemType != GetItemRegistry().Get(LastItemCollected.GetValue())) && (
		itemCollectedTag == ItemTag::Food ||
		itemCollectedTag == ItemTag::Potion ||
		itemType.getId() == game::item::type::Trident.getId() ||
		itemType.getId() == game::item::type::TNTBox.getId() ||
		itemType.getId() == game::item::type::Arrow.getId() ||
		itemSlotType == ESlotType::Consumable);
}
