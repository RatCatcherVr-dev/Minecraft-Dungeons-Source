#include "Dungeons.h"
#include "game/Enchantments/Enchantment.h"
#include "game/Enchantments/EnchantmentUtil.h"
#include "game/item/power/ItemPowerUtil.h"
#include "game/item/ItemType.h"
#include "game/item/instance/AItemInstance.h"
#include "game/item/ItemArchetypeCounts.h"
#include "game/ArmorProperties/ArmorPropertyTypeDefs.h"
#include "util/Algo.h"
#include "InventoryItemUtil.h"

FText UInventoryItemUtil::GetDisplayItemPowerText(const FInventoryItemData& Item) {
	return UItemPowerUtil::GetItemPowerDisplayText(Item.ItemPower);
}

int UInventoryItemUtil::GetDisplayItemPowerInt(const FInventoryItemData& Item) {
	return UItemPowerUtil::GetItemPowerDisplayInt(Item.ItemPower);
}

EItemRarity UInventoryItemUtil::GetDisplayRarity(const FInventoryItemData& Item) {
	return Item.Rarity;
}

bool UInventoryItemUtil::GetShouldDisplayPower(const FInventoryItemData& Item) {
	return GetItemRegistry().Get(Item.GetItemId()).usesItemPower();
}

bool UInventoryItemUtil::GetShouldDisplayRarity(const FInventoryItemData& Item) {
	return GetItemRegistry().Get(Item.GetItemId()).usesRarity();
}

bool UInventoryItemUtil::GetIsNetherite(const FInventoryItemData& Item) {
	return Item.IsNetherite();
}

int UInventoryItemUtil::GetTotalInvestedEnchantmentPoints(const FInventoryItemData& Item) {
	int points = 0;
	for (auto e : Item.Enchantments) {
		points += algo::sum(range_incl(1, e.Level), RETLAMBDA(UEnchantment::GetUpgradeCostForEnchantmentTypeLevel(e.TypeID, it, Item.IsNetherite())));
	}

	return points;
}

int UInventoryItemUtil::GetNumberOfEnchantmentSlots(const FInventoryItemData& Item) {
	return Item.GetNumEnchantmentSlots();
}

int UInventoryItemUtil::GetNumberOfEnchantmentsInSlot(const FInventoryItemData& Item, int enchantmentSlot) {
	return Item.GetNumEnchantmentsInSlotIndex(enchantmentSlot);
}

bool UInventoryItemUtil::DoesEnchantmentSlotContainAnyOfRarity(const FInventoryItemData& Item, int enchantmentSlot, EEnchantmentRarity rarity) {
	return algo::any_of(Item.GetValidEnchantmentsInSlotIndex(enchantmentSlot), RETLAMBDA(game::enchantment::type::getEnchantmentType(it->TypeID).isRarity(rarity);));
}

const FText& UInventoryItemUtil::GetDisplayNameText(const FInventoryItemData& Item) {
	return GetItemRegistry().Get(Item.GetItemId()).getNameText();
}

const FText& UInventoryItemUtil::GetDisplayDescriptionText(const FInventoryItemData& Item) {
	return GetItemRegistry().Get(Item.GetItemId()).getDescriptionText();
}

const FText& UInventoryItemUtil::GetDisplayFlavourText(const FInventoryItemData& Item) {
	return GetItemRegistry().Get(Item.GetItemId()).getFlavourText();
}


#define LOCTEXT_NAMESPACE "ItemType"

TArray<FItemCharacteristic> UInventoryItemUtil::GetDisplayCharacteristicsText(const FInventoryItemData& Item) {
	TArray<FItemCharacteristic> DisplayCharacteristics;

	auto&& itemType = GetItemRegistry().Get(Item.GetItemId());
	const auto characteristics = itemType.getCharacteristics();
	if (characteristics.Num() > 0) {
		for (auto characteristic : characteristics) {
			DisplayCharacteristics.Add(characteristic);
		}
	}

	if (Item.ArmorProperties.Num() > 0) {
		for (auto prop : Item.ArmorProperties) {
			auto& armorPropertyType = game::armorproperties::type::getArmorPropertyType(prop.ID);
			const FText text = armorPropertyType.getCharacteristicText();
			DisplayCharacteristics.Add({ text, prop.Rarity });
		}
	}

	const auto enchantmentCharacteristics = itemType.getEnchantmentCharacteristics();
	if (enchantmentCharacteristics.Num() > 0) {
		for (auto characteristic : enchantmentCharacteristics) {
			DisplayCharacteristics.Add(characteristic);
		}
	}

	if (Item.IsNetherite()) {
		DisplayCharacteristics.Emplace(game::enchantment::type::getEnchantmentType(Item.NetheriteEnchant().TypeID).getCharacteristicText(), true, Item.NetheriteEnchant());
	}

	return DisplayCharacteristics;
}

bool UInventoryItemUtil::GetIsMysteryBoxItem(const FInventoryItemData & Item)
{
	return GetItemRegistry().Get(Item.GetItemId()).getInstanceClass().GetDefaultObject()->GetName().Contains("Mystery") ? true : false;
}

bool UInventoryItemUtil::GetIsEventItem(const FInventoryItemData& Item) {
	return Item.GetItemType().isEventItem();
}

EItemEventType UInventoryItemUtil::GetItemEventType(const FInventoryItemData& Item) {
	return Item.GetItemType().getEventType();
}

FItemBulletPoint UInventoryItemUtil::CreateSoulGatherBulletPoint(int soulGatherCount) {
	auto displayValue = valueformat::asRelativeConstant((int)soulGatherCount);
	return { FText::Format(LOCTEXT("x_soul_gathering", "{0} Soul Gathering"), FText::FromString(displayValue)), EBulletIcon::SoulGather };
}

const TArray<EItemArchetype>& UInventoryItemUtil::GetItemArchetypes(const FInventoryItemData& Item) {
	return Item.GetItemType().getArchetypes();
}

TMap<EItemArchetype, int> UInventoryItemUtil::GetItemsArchetypeCounts(const TArray<FInventoryItemData>& Items) {
	TMap<EItemArchetype, int> counts;
	for (auto pair : ItemArchetypeCounts(Items).counts) {
		counts.Add(pair.first, pair.second);
	}
	return counts;
}

TArray<FItemBulletPoint> UInventoryItemUtil::GetDisplayBulletPointsText(ABaseCharacter* owner, const FInventoryItemData& Item)
{
	TArray<FItemBulletPoint> DisplayBullets;
	auto&& itemType = GetItemRegistry().Get(Item.GetItemId());

	const AItemInstance* instance = Cast<const AItemInstance>(itemType.getInstanceClass()->GetDefaultObject());

	auto&& powereffects = instance->CreatePowerEffectDescription(*owner, Item.ItemPower);
	for (auto &&powereffect : powereffects) {
		DisplayBullets.Add({ powereffect, EBulletIcon::ItemPower, itemType.getTag(), Item.bIsUpgraded });
	}

	instance->PopulateBulletPoints(DisplayBullets, *owner);

	auto characteristics = GetDisplayCharacteristicsText(Item);

	for (auto&& characteristic : characteristics) {
		DisplayBullets.Add(FItemBulletPoint::CreateCharacteristicBulletPoint(characteristic));
	}

	if (itemType.hasCooldown()) {
		DisplayBullets.Add(FItemBulletPoint::CreateCooldownBulletPoint(itemType.getCoolDownSeconds()));
	}

	{
		int soulGatherCount = 0;
		soulGatherCount += itemType.getSoulGatherCount();

		for (auto&& enchantmentData : itemType.getDefaultEnchantments()) {
			soulGatherCount += game::enchantment::type::getEnchantmentType(enchantmentData.TypeID).getSoulGatherCount();
		}

		if (soulGatherCount > 0) {
			DisplayBullets.Add(FItemBulletPoint::CreateSoulGatherBulletPoint(soulGatherCount));
		}
	}

	if (itemType.isSoulUseItem()) {
		DisplayBullets.Add({ LOCTEXT("soul_use_item", "Requires Souls"), EBulletIcon::SoulUse });
	}

	if (itemType.isEventItem()) {
		DisplayBullets.Add(FItemBulletPoint::CreateSeasonalLimitedBulletPoint());
	}

	return DisplayBullets;
}

#undef LOCTEXT_NAMESPACE


FText UInventoryItemUtil::GetDisplayItemNameAndPowerText(const FInventoryItemData& Item) {
	auto&& ItemNameText = GetDisplayNameText(Item);
	auto&& ItemPowerText = GetDisplayItemPowerText(Item);
	return FText::Format(FText::FromString("{0} ({1})"), ItemNameText, ItemPowerText);
}

bool UInventoryItemUtil::NotEqual_InventoryItemDataArray(const TArray<FInventoryItemData>& A, const TArray<FInventoryItemData>& B) {
	return A != B;
}
