#include "Dungeons.h"
#include "InventoryItemData.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/component/EnchantmentComponent.h"
#include "util/Algo.h"

FInventoryItemData::FInventoryItemData()
	: ItemPower(1)
	, Rarity(EItemRarity::Common)
	, bIsUpgraded(false)
	, bIsGifted(false)
	, bIsModified(false) {
}

FInventoryItemData::FInventoryItemData(const FItemId& type, float itemPower, TArray<FEnchantmentData> enchantments, TOptional<FEnchantmentData> netherite,
	TArray<FArmorPropertyData> armorProperties, EItemRarity rarity,  bool isUpgraded, bool isGifted, bool isModified)
	: ItemId(type)
	, ItemPower(itemPower)
	, Enchantments(std::move(enchantments))
	, ArmorProperties(std::move(armorProperties))
	, Rarity(rarity)
	, bIsUpgraded(isUpgraded)
	, bIsGifted(isGifted)
	, bIsModified(isModified)
{
	bHasNetherite = netherite.IsSet();
	if(bHasNetherite) {
		NetheriteEnchantData = netherite.GetValue();
		NetheriteEnchantData.Source = EEnchantmentSource::Netherite;
	}
}

bool FInventoryItemData::operator<(const FInventoryItemData& other) const {
	return ItemId < other.ItemId;
}

bool FInventoryItemData::operator==(const FInventoryItemData& other) const {
	return ItemId == other.ItemId
		&& ItemPower == other.ItemPower
		&& Enchantments == other.Enchantments
		&& ArmorProperties == other.ArmorProperties
		&& Rarity == other.Rarity
		&& bIsGifted == other.bIsGifted
		&& bIsUpgraded == other.bIsUpgraded
		&& bIsModified == other.bIsModified
		&& NetheriteEnchantData == other.NetheriteEnchantData
		&& OverrideStoreCount == other.OverrideStoreCount;
}

const ItemType& FInventoryItemData::GetItemType() const {
	return GetItemRegistry().Get(ItemId);
}

int FInventoryItemData::GetNumEnchantmentSlots() const
{
	const int highestIndex = UEnchantmentComponent::HighestIndex(Enchantments);
	//index of -1 should become 0, index of 2 should become 1.
	return (highestIndex + game::enchantment::RowSize) / game::enchantment::RowSize;
}

namespace enchantmentslots {
	TArray<int> SlotsIndexToEnchantmentIndexes(int slotIndex) {
		int startIndex = slotIndex * game::enchantment::RowSize;
		int endIndexExclusive = startIndex + game::enchantment::RowSize;
		return algo::iota_tarray<int>(startIndex, endIndexExclusive);
	}
}

bool FInventoryItemData::IsValidEnchantmentIndex(int enchantmentIndex) const {
	return Enchantments.IsValidIndex(enchantmentIndex) && Enchantments[enchantmentIndex].TypeID != EEnchantmentTypeID::Unset;
}

int FInventoryItemData::GetNumEnchantmentsInSlotIndex(int slotIndex) const {
	return algo::count_if(enchantmentslots::SlotsIndexToEnchantmentIndexes(slotIndex), RETLAMBDA(IsValidEnchantmentIndex(it)));
}

FInventoryItemData::SlotEnchantmentData FInventoryItemData::GetValidEnchantmentsInSlotIndex(int slotIndex) const {
	return algo::copy_if_map_as<SlotEnchantmentData>(enchantmentslots::SlotsIndexToEnchantmentIndexes(slotIndex), RETLAMBDA(IsValidEnchantmentIndex(it)), RETLAMBDA(&Enchantments[it]));
}

TArray<FInventoryItemData::SlotEnchantmentData> FInventoryItemData::GetValidEnchantmentsSlotData() const {
	return algo::map_as<TArray<SlotEnchantmentData>>(algo::iota_tarray<int>(0, GetNumEnchantmentSlots()), RETLAMBDA(GetValidEnchantmentsInSlotIndex(it)));
}

void FInventoryItemData::ResetEnchantmentLevels() {
	for (auto& enchantment : Enchantments) {
		enchantment.Level = 0;
	}
}

void FInventoryItemData::OnWasAdded(UInventoryItem* item) {
	if(OnAdded.IsBound()) {
		OnAdded.ExecuteIfBound(item);
		OnAdded.Unbind();
	}
}

bool FInventoryItemData::IsUpgraded() const {
	return bIsUpgraded;
}

bool FInventoryItemData::IsGifted() const {
	return bIsGifted;
}

bool FInventoryItemData::IsModified() const {
	return bIsModified;
}

TArray<FEnchantmentData> FInventoryItemData::AllEnchantments() const {
	TArray<FEnchantmentData> out = Enchantments;
	for (FEnchantmentData& enchantment : out) {
		enchantment.Source = EEnchantmentSource::Generated;
	}

	TArray<FEnchantmentData> defaults = GetItemType().getDefaultEnchantments();
	for (FEnchantmentData& enchantment : defaults) {
		enchantment.Source = EEnchantmentSource::Permanent;
	}
	out.Append(defaults);
	
	if (bHasNetherite) {
		out.Add(NetheriteEnchantData);
	}

	return out;
}

TArray<FEnchantmentDataWithRarity> FInventoryItemData::AllEnchantmentsWithRarity() const {
	TArray<FEnchantmentData> out = AllEnchantments();
	TArray<FEnchantmentDataWithRarity> out2;
	for (FEnchantmentData& enchantment : out) {
		out2.Add({ enchantment, Rarity });
	}
	return out2;
}

int FInventoryItemData::GetStoreCount() const {
	return OverrideStoreCount.Get(GetItemType().getStoreCount());
}

int UInventoryItemDataFunctionLibrary::GetStoreCount(const FInventoryItemData& itemData) {
	return itemData.GetStoreCount();
}
