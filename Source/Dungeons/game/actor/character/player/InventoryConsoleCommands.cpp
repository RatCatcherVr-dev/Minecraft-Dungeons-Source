#include "Dungeons.h"
#include "DungeonsGameInstance.h"
#include "BasePlayerController.h"
#include "game/CommandConsoleCommands.h"
#include "game/item/ItemType.h"
#include "game/util/ConsoleCommandHelpers.h"
#include "util/Algo.h"
#include "util/EnumUtil.h"
#include <HAL/ConsoleManager.h>
#include "save/CharacterSaveData.h"
#include "game/component/CharacterSerializeComponent.h"
#include "game/item/generator/ItemGenerator.h"
#include "game/item/enchantment/EnchantmentData.h"

static std::vector<ItemSaveData>& GetSavedInventoryFromController(ABasePlayerController* playerController, TOptional<int> index) {
	return playerController->GetCharacterSerializeComponent()->GetItems();
}

void RefreshInventory(UWorld* world, TOptional<int> index) { // @todo: handle index?

	if (ABasePlayerController* playerController = Cast<ABasePlayerController>(world->GetFirstPlayerController())) {
		if (auto* itemStash = playerController->GetItemStashComponent()) {
			itemStash->DeserializeSaveState();
			itemStash->RefreshCachedPoints();
		}
	}
}

TOptional<int> GetOptionalPlayerId(const TArray<FString>& args, int argIndex) {
	return ArgAsInt(args, 0);
}

struct ParseItemResult {
	ParseItemResult(FInventoryItemData item, TOptional<EEquipmentSlot> slot = {}) : item(std::move(item)), hotbarSlot(slot) {}
	ParseItemResult(FString error) : error(error) {}

	TOptional<FInventoryItemData> item;
	TOptional<EEquipmentSlot> hotbarSlot;
	FString error;
	explicit operator bool() const { return item.IsSet(); }
};

 ParseItemResult ParseItem(const TArray<FString>& args, int startIndex) {
	int index = startIndex;

	//
	// Parse optional HotbarSlot
	//
	TOptional<EEquipmentSlot> hotbarSlot;
	if (auto optionalHotbarSlot = EnumValueFromString(EEquipmentSlot, ArgAsFString(args, index).Get(""))) {
		hotbarSlot = optionalHotbarSlot.GetValue();
		index++;
	}
	//
	// Parse optional rarity
	//
	EItemRarity rarity = EItemRarity::Common;
	if (auto optionalRarity = EnumValueFromString(EItemRarity, ArgAsFString(args, index).Get(""))) {
		rarity = optionalRarity.GetValue();
		index++;
	}
	//
	// Parse item-type and item-power
	//
	if (index + 2 > args.Num()) { // 'type power' is obligatory
		return FString("'EItemType item-power' arguments are needed");
	}
	auto type = GetItemRegistry().Request(*args[index++]);
	if (!type) {
		return "Failed to parse EItemType: " + args[index - 1];
	}
	auto power = ArgAsFloat(args, index++);
	if (!power) {
		return "Failed to parse item power: " + args[index - 1];
	}
	//
	// Parse enchantment-type enchantment-level pairs
	//
	int remainingArgs = args.Num() - index;
	if (remainingArgs % 2 == 1) {
		return FString("Uneven number of enchantment arguments (needs to be a list of zero to three 'enchantment-type enchantment-level' pairs)");
	}
	TArray<FEnchantmentData> enchantments;
	TOptional<FEnchantmentData> netheriteEnchant;

	auto calculateFirstNextUpgradedIndexForLastIndex = [](int index) { return 3 * (1 + (index / 3)); };
	auto addUnsets = [&enchantments](int count) { if (count > 0) enchantments.AddDefaulted(count); };

	int lastUpgradedIndex = -999; // hack

	while (index < args.Num()) {
		bool netherite = args[index].Equals("netherite", ESearchCase::IgnoreCase);

		if (netherite) ++index;

		auto enchantment = EnumValueFromString(EEnchantmentTypeID, args[index++]);
		if (!enchantment) {
			return "Failed to parse EEnchantmentTypeID: " + args[index-1];
		}
		auto enchantmentLevel = ArgAsInt(args, index++);
		if (!enchantmentLevel) {
			return "Failed to parse enchantment level: " + args[index-1];
		}

		if (netherite) {
			netheriteEnchant.Emplace(enchantment.GetValue(), enchantmentLevel.GetValue());
		}
		else {
			if (enchantmentLevel.GetValue()) {
				const int nextIndex = calculateFirstNextUpgradedIndexForLastIndex(lastUpgradedIndex);
				addUnsets(nextIndex - enchantments.Num());
				lastUpgradedIndex = enchantments.Num();
			}
			enchantments.Emplace(enchantment.GetValue(), enchantmentLevel.GetValue());
		}
	}
	// Fill the rest of the enchantment row
	addUnsets((33333 - enchantments.Num()) % 3);

	return { game::item::generator::generate(type.GetValue(), power.GetValue(), enchantments, rarity, netheriteEnchant), hotbarSlot };
};

//
// CLEAR
//
static void DoClearInventory(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	auto id = GetOptionalPlayerId(args, 0);

	if (ABasePlayerController* playerController = Cast<ABasePlayerController>(world->GetFirstPlayerController())) {
		auto& inventory = GetSavedInventoryFromController(playerController, id);
		inventory.clear();

		RefreshInventory(world, id);
	}
}

static const FAutoConsoleCommand ClearInventoryCommand(TEXT("Dungeons.Inventory.Clear")
	, TEXT("Clears the inventory")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoClearInventory)
	, ECVF_Cheat);


//
// STORE
//
static void DoStoreInventory(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	auto id = GetOptionalPlayerId(args, 0);
	auto item = ParseItem(args, id ? 1 : 0);

	if (!item) {
		out.Log("Can't parse item: " + item.error);
		return;
	}

	if (ABasePlayerController* playerController = Cast<ABasePlayerController>(world->GetFirstPlayerController())) {
		auto& inventory = GetSavedInventoryFromController(playerController, id);
		auto occupiedIndices = algo::copy_if_map_as<std::unordered_set<unsigned>>(inventory, RETLAMBDA(!it.Equipped()), RETLAMBDA(it.GetIndex()));
		auto firstEmptyIndex = [&set = occupiedIndices] {
			for (int i = 0; i < set.size(); ++i) {
				if (set.find(i) == set.end()) { return i; }
			}
			return static_cast<int>(set.size());
		}();
		inventory.push_back(createItem(item.item.GetValue(), firstEmptyIndex));

		RefreshInventory(world, id);
	}
}

static const FAutoConsoleCommand StoreInventoryCommand(TEXT("Dungeons.Inventory.Store")
	, TEXT("Stores an item in the inventory: Dungeons.Inventory.Store [rarity] type power (enchantment enchantment-level){0,3}")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoStoreInventory)
	, ECVF_Cheat);


//
// EQUIP
//
EEquipmentSlot GetSlot(const ParseItemResult& item) {
	EquipmentSlotter slotter;
	const auto autoSlot = slotter(GetItemRegistry().Get(item.item->GetItemId()).slotType());

	if (isHotbarSlot(autoSlot) && item.hotbarSlot && isHotbarSlot(item.hotbarSlot.GetValue())) {
		return item.hotbarSlot.GetValue();
	}
	return autoSlot;
}

static void DoEquipInventory(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	auto id = GetOptionalPlayerId(args, 0);
	auto item = ParseItem(args, id ? 1 : 0);

	if (!item) {
		out.Log("Can't parse item: " + item.error);
		return;
	}
	
	if (ABasePlayerController* playerController = Cast<ABasePlayerController>(world->GetFirstPlayerController())) {
		auto& inventory = GetSavedInventoryFromController(playerController, id);
		auto slot = GetSlot(item);
		auto inventoryItem = createItem(item.item.GetValue(), slot);
		inventory = algo::copy_if(inventory, RETLAMBDA(!(it.Equipped() && it.GetEquipmentSlot() == slot)));
		inventory.push_back(inventoryItem);

		RefreshInventory(world, id);
	}
}

static const FAutoConsoleCommand EquipInventoryCommand(TEXT("Dungeons.Inventory.Equip")
	, TEXT("Equips an item: Dungeons.Inventory.Equip [hotbarslot1/2/3] [rarity] type power (enchantment enchantment-level){0,3}")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoEquipInventory)
	, ECVF_Cheat);


//
// EMIT
//
FString InventoryItemString(const FInventoryItemData& item) {
	TArray<FString> words{
		item.Rarity == EItemRarity::Common? "" : GetEnumValueToStringStripped(item.Rarity),
		item.GetItemId().ToString(),
		FString::SanitizeFloat(item.ItemPower, 0)
	};
	algo::map_to(item.Enchantments,
		RETLAMBDA(GetEnumValueToStringStripped(it.TypeID) + " " + FString::FromInt(it.Level)),
		words
	);
	return FString::Join(algo::copy_if(words, RETLAMBDA(!it.IsEmpty())), TEXT(" "));
}

FString StoreItemString(UInventoryItemSlot* slot) {
	if (slot->Item) {
		return "Dungeons.Inventory.Store " + InventoryItemString(slot->Item->Item);
	}
	return "";
}

FString EquipItemString(const TPair<EEquipmentSlot, UInventoryItemSlot*>& slot) {
	if (slot.Value->Item) {
		const auto slotString = isHotbarSlot(slot.Key) ? (GetEnumValueToStringStripped(slot.Key) + " ") : "";
		return "Dungeons.Inventory.Equip " + slotString + InventoryItemString(slot.Value->Item->Item);
	}
	return "";
}

TArray<FString> EmitInventoryCommands(UItemStashComponent& component) {
	TArray<FString> commands{ "Dungeons.Inventory.Clear" };
	algo::map_to(component.GetEquipmentSlots(), EquipItemString, commands);
	algo::map_to(component.GetInventorySlots(), StoreItemString, commands);
	return commands;
}

static void DoEmitInventory(const TArray<FString>& args, UWorld* world, FOutputDevice& out) {
	auto* inventory = GetPlayerComponent<UItemStashComponent>(world, args);
	if (!inventory) {
		out.Log("Can't find player controller or an UItemStashComponent on it");
		return;
	}
	out.Log(GenerateRunCommandsString(EmitInventoryCommands(*inventory)));
}

static const FAutoConsoleCommand EmitInventoryCommand(TEXT("Dungeons.Inventory.Emit")
	, TEXT("Emits a command representing your inventory (including equipped)")
	, FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&DoEmitInventory)
	, ECVF_Cheat);
