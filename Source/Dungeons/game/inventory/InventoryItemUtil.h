#pragma once
#include <Kismet/BlueprintFunctionLibrary.h>
#include "game/item/InventoryItemData.h"
#include "game/item/ItemCharacteristic.h"
#include "game/item/ItemBulletPoint.h"
#include "game/item/ItemRarity.h"
#include "game/item/ItemArchetype.h"
#include "InventoryItemUtil.generated.h"

class ABaseCharacter;

UCLASS()
class DUNGEONS_API UInventoryItemUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetTotalInvestedEnchantmentPoints(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetNumberOfEnchantmentSlots(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetNumberOfEnchantmentsInSlot(const FInventoryItemData& Item, int enchantmentSlot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool DoesEnchantmentSlotContainAnyOfRarity(const FInventoryItemData& Item, int enchantmentSlot, EEnchantmentRarity rarity);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetDisplayNameText(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetDisplayDescriptionText(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const FText& GetDisplayFlavourText(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static TArray<FItemCharacteristic> GetDisplayCharacteristicsText(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static TArray<FItemBulletPoint> GetDisplayBulletPointsText(ABaseCharacter* owner, const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetDisplayItemPowerText(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static FText GetDisplayItemNameAndPowerText(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static int GetDisplayItemPowerInt(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static EItemRarity GetDisplayRarity(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetShouldDisplayPower(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetShouldDisplayRarity(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetIsNetherite(const FInventoryItemData& Item);	
	static bool GetIsMysteryBoxItem(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static bool GetIsEventItem(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static EItemEventType GetItemEventType(const FInventoryItemData& Item);

	FItemBulletPoint CreateSoulGatherBulletPoint(int soulGatherCount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static const TArray<EItemArchetype>& GetItemArchetypes(const FInventoryItemData& Item);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons")
	static TMap<EItemArchetype, int> GetItemsArchetypeCounts(const TArray<FInventoryItemData>& Items);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "NotEqual (Array<InventoryItemData>)", CompactNodeTitle = "!="), Category = "Dungeons|InventoryItem")
	static bool NotEqual_InventoryItemDataArray(const TArray<FInventoryItemData>& A, const TArray<FInventoryItemData>& B);
};
