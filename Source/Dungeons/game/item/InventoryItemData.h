#pragma once

#include <kismet/BlueprintFunctionLibrary.h>
#include "ItemRarity.h"
#include "SerializableItemId.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "game/ArmorProperties/ArmorPropertyData.h"
#include "InventoryItemData.generated.h"

class UInventoryItem;
DECLARE_DELEGATE_OneParam(FInventoryItemDataAddedAsInventoryItem, UInventoryItem*);

USTRUCT(BlueprintType)
struct DUNGEONS_API FInventoryItemData {
	GENERATED_USTRUCT_BODY()

	FInventoryItemData();

	FInventoryItemData(const FItemId& type, float itemPower = 1, TArray<FEnchantmentData> enchantments = {}, TOptional<FEnchantmentData> netherite = {}, TArray<FArmorPropertyData> armorProperties = {}, EItemRarity rarity = EItemRarity::Common, bool isUpgraded = false, bool gifted = false, bool isModified = false);

	bool operator<(const FInventoryItemData&) const;
	bool operator==(const FInventoryItemData&) const;

	const FSerializableItemId& GetItemId() const { return ItemId; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	FSerializableItemId ItemId;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float ItemPower;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	TArray<FEnchantmentData> Enchantments;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	TArray<FArmorPropertyData> ArmorProperties;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	EItemRarity Rarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	bool bIsUpgraded;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	bool bIsGifted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	bool bIsModified;

	TOptional<int> SubItemID;

	TOptional<int> OverrideStoreCount;

	const ItemType& GetItemType() const;

	int GetNumEnchantmentSlots() const;

	int GetNumEnchantmentsInSlotIndex(int) const;
	using SlotEnchantmentData = TArray<const FEnchantmentData*>;
	SlotEnchantmentData GetValidEnchantmentsInSlotIndex(int) const;	
	TArray<SlotEnchantmentData> GetValidEnchantmentsSlotData() const;

	void ResetEnchantmentLevels();

	FInventoryItemDataAddedAsInventoryItem OnAdded;
	void OnWasAdded(UInventoryItem*);

	bool IsUpgraded() const;

	bool IsGifted() const;

	bool IsNetherite() const { return bHasNetherite; }

	bool IsModified() const;

	//Combined array of all enchantments this item has, permanent, generated and other sources.
	TArray<FEnchantmentData> AllEnchantments() const;
	TArray<FEnchantmentDataWithRarity> AllEnchantmentsWithRarity() const;

	int GetStoreCount() const;

	const FEnchantmentData& NetheriteEnchant() const { return NetheriteEnchantData; };
private:
	
	bool IsValidEnchantmentIndex(int enchantmentIndex) const;
	
	UPROPERTY()
	bool bHasNetherite = false;

	UPROPERTY()
	FEnchantmentData NetheriteEnchantData; 
};

UCLASS()
class DUNGEONS_API UInventoryItemDataFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetStoreCount(const FInventoryItemData& itemData);
};

//Information about the item in respect to the inventory
USTRUCT(BlueprintType)
struct DUNGEONS_API FInventoryItemMetaData {
	GENERATED_USTRUCT_BODY()

	FInventoryItemMetaData(bool markedNew = true)
		: MarkedNew(markedNew)
		, Cloned(false)
	{ 
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	bool MarkedNew;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dungeons")
	bool Cloned;
};
