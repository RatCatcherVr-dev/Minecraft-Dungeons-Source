#include "Dungeons.h"
#include "InventoryItem.h"
#include "StatTracker.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/ItemStashComponent.h"
#include "game/item/ItemType.h"
#include "game/Enchantments/Enchantment.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/inventory/InventoryItemUtil.h"

extern TAutoConsoleVariable<int32> CVarUnlimitedEnchantment;

bool UInventoryItem::EnchantIndex(int32 index) {
	if (!CanEnchantIndex(index)) {
		OnItemEnchanted.Broadcast(false, index);
		return false;
	}

	Item.Enchantments[index].Level++;

	if(ItemStashComponent != nullptr){ //items are technically not always in an itemstashcomponent.
		ItemStashComponent->SerializeSaveState();		
		ItemStashComponent->RefreshCachedPoints();
	}

	if (OnItemChangedInternal.IsBound()) {
		OnItemChangedInternal.Broadcast();
	}
	OnItemChanged.Broadcast();
	OnItemEnchanted.Broadcast(true, index);

	// D11.SSN
	if (auto* tracker = Cast<APlayerCharacter>(ItemStashComponent->GetOwner())->GetStatTracker()) {
		tracker->Enchant(Item.Enchantments[index].Level);
	}

	return true;
}

bool UInventoryItem::ReplaceEnchantment(const int32 index, const FEnchantmentData& enchantment)
{
	if (Item.Enchantments[index].Level > 0) {
		return false;
	}

	Item.Enchantments[index] = enchantment;
	Item.bIsModified = true;
	
	if(ItemStashComponent != nullptr){ //items are technically not always in an itemstashcomponent.
		ItemStashComponent->SerializeSaveState();		
		ItemStashComponent->RefreshCachedPoints();
	}

	if (OnItemChangedInternal.IsBound()) {
		OnItemChangedInternal.Broadcast();
	}
	OnItemChanged.Broadcast();
	OnItemEnchantmentChanged.Broadcast(index);

	return true;
}

int UInventoryItem::GetTotalInvestedEnchantmentPoints() const {
	return UInventoryItemUtil::GetTotalInvestedEnchantmentPoints(Item);
}

int UInventoryItem::GetTotalEnchantmentRows() const {
	return Item.GetNumEnchantmentSlots();
}

bool UInventoryItem::IsEnchantIndexLocked(int32 index) const {
	//Enchantments are now always unlocked - all slots
	return false;
}

//Is index outside enchantments array or is it unset
bool UInventoryItem::IsEnchantIndexHidden(int32 index) const {
	if (Item.Enchantments.Num() <= index) return true;
	return (Item.Enchantments[index].TypeID == EEnchantmentTypeID::Unset);	
}

//Disabled, due to investment action
bool UInventoryItem::IsEnchantIndexDisabled(int32 index) const {
	if (Item.Enchantments.Num() <= index) return false;

	const int rowSize = game::enchantment::RowSize;
	const int rowIndex = index / rowSize;

	const bool rowAvailable = rowIndex <= (game::enchantment::UnlockedRows - 1);

	bool hasPointInAnotherEnchantmentInRow = false;

	if (rowAvailable) {
		// Start on beginning of row, end on last index of row
		for (int i = rowIndex * rowSize; i < (rowIndex * rowSize) + rowSize; ++i) {
			if (index != i) {
				if (Item.Enchantments[i].Level > 0) {
					hasPointInAnotherEnchantmentInRow = true;
					break;
				}
			}
		}
	}

	if (hasPointInAnotherEnchantmentInRow) return true;

	return false;
}

//Maxed, investments cannot go higher.
bool UInventoryItem::IsEnchantIndexMaxed(int32 index) const {
	if (Item.Enchantments.Num() <= index) return false;

	if (Item.Enchantments[index].Level >= game::enchantment::MaxPointsPerEnchantment) return true;
	return false;
}

//Any level invested in.
bool UInventoryItem::IsEnchantIndexEnchanted(int32 index) const {
	if (Item.Enchantments.Num() <= index) return false;

	if (Item.Enchantments[index].TypeID != EEnchantmentTypeID::Unset && Item.Enchantments[index].Level > 0) return true;
	return false;
}

bool UInventoryItem::IsEnchantIndexAffordable(int32 index) const {
	if (Item.Enchantments.Num() <= index) return false;

	auto enchantmentData = Item.Enchantments[index];
	auto cost = UEnchantment::GetUpgradeCostForEnchantmentTypeLevel(enchantmentData.TypeID, enchantmentData.Level + 1, Item.IsNetherite());
	bool hasEnoughPoints = CVarUnlimitedEnchantment.GetValueOnGameThread() || (ItemStashComponent ? (ItemStashComponent->AvailableEnchantmentPoints() >= cost) : false);
	
	return hasEnoughPoints;
}

bool UInventoryItem::CanEnchantIndex(int32 index) const {
	if (!CanEnchant()) { return false; }
	if (Item.Enchantments.Num() <= index) return false;

	if (!this->IsEnchantIndexAffordable(index) || this->IsEnchantIndexLocked(index) || this->IsEnchantIndexDisabled(index) || this->IsEnchantIndexMaxed(index) ) return false;
	return true;
}

bool UInventoryItem::Enchantable() const {
	return GetItemRegistry().Get(Item.GetItemId()).isGear();
}

bool UInventoryItem::IsGear() const {
	return GetItemRegistry().Get(Item.GetItemId()).isGear();
}

bool UInventoryItem::IsGifted() const {
	return Item.IsGifted();
}

bool UInventoryItem::IsNetherite() const {
	return Item.IsNetherite();
}

bool UInventoryItem::HasTag(ItemTag tag) const {
	return GetItemRegistry().Get(Item.GetItemId()).hasTag(tag);
}

bool UInventoryItem::IsCloned() const {
	return Meta.Cloned;
}

ItemTag UInventoryItem::GetTag() const {
	return GetItemRegistry().Get(Item.GetItemId()).getTag();
}

bool UInventoryItem::IsInventoryOnly() const {
	return GetItemRegistry().Get(Item.GetItemId()).isInventoryOnly();
}


bool UInventoryItem::CanBeUpgraded() const {
	return !Item.bIsUpgraded && GetItemRegistry().Get(Item.GetItemId()).isUpgradable();
}

void UInventoryItem::SetItemPower(float power) {
	Item.ItemPower = power;

	if (ItemStashComponent != nullptr) { //items are technically not always in an itemstashcomponent.
		ItemStashComponent->SerializeSaveState();
	}
	if (OnItemChangedInternal.IsBound()) { OnItemChangedInternal.Broadcast(); }
	OnItemChanged.Broadcast();
}

bool UInventoryItem::GetIsUpgraded() const {
	return Item.bIsUpgraded;
}

const float UInventoryItem::DIAMOND_DUST_UPGRADE_AMOUNT = 0.2f;

bool UInventoryItem::TryUpgradeItem() {
	if( CanBeUpgraded() ){
		Item.bIsUpgraded = true;

		SetItemPower(GetItemPower() + DIAMOND_DUST_UPGRADE_AMOUNT);		

		if (OnItemChangedInternal.IsBound()) { OnItemChangedInternal.Broadcast(); }
		OnItemChanged.Broadcast();

		return true;
	}
	return false;
}





bool UInventoryItem::CanSalvage() const {
	return ItemStashComponent != nullptr;
}

bool UInventoryItem::CanEnchant() const {
	return ItemStashComponent != nullptr;
}

const FInventoryItemData& UInventoryItem::GetItemData() const {
	return Item;
}

bool UInventoryItem::IsNew() const {	
	return Meta.MarkedNew;
}

FText UInventoryItem::GetDisplayItemPowerText() const {
	return UInventoryItemUtil::GetDisplayItemPowerText(Item);
}

int UInventoryItem::GetDisplayItemPowerInt() const {
	return UInventoryItemUtil::GetDisplayItemPowerInt(Item);
}

float UInventoryItem::GetItemPower() const
{
	return Item.ItemPower;
}

const FText& UInventoryItem::GetDisplayNameText() const {
	return UInventoryItemUtil::GetDisplayNameText(Item);
}

const FText& UInventoryItem::GetDisplayDescriptionText() const {
	return UInventoryItemUtil::GetDisplayDescriptionText(Item);
}

const FText& UInventoryItem::GetDisplayFlavourText() const {
	return UInventoryItemUtil::GetDisplayFlavourText(Item);
}

const TArray<FItemCharacteristic>& UInventoryItem::GetDisplayCharacteristicsText() {
	DisplayCharacteristics = UInventoryItemUtil::GetDisplayCharacteristicsText(Item);
	return DisplayCharacteristics;
}

const TArray<FItemBulletPoint>& UInventoryItem::GetDisplayBulletPointsText(ABaseCharacter* owner){
	DisplayBulletPoints = UInventoryItemUtil::GetDisplayBulletPointsText(owner, Item);
	return DisplayBulletPoints;
}



bool UInventoryItem::IsMarkedNew() const {
	return Meta.MarkedNew;
}

void UInventoryItem::ClearMarkedNew() {
	Meta.MarkedNew = false;
	OnMetaChanged.Broadcast();

	if (ItemStashComponent != nullptr) { //items are technically not always in an itemstashcomponent.
		ItemStashComponent->RefreshNumNewItems();
	}
}

bool UInventoryItem::IsBeingOffered() const {
	if (ItemStashComponent) {
		return ItemStashComponent->IsItemBeingOffered(this);
	}
	return false;
}
