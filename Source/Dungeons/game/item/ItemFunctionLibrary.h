#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "game/item/ItemTypeDefs.h"
#include "Engine/Texture2D.h"
#include "SerializableItemId.h"
#include "ItemFunctionLibrary.generated.h"
/**
 * 
 */
UCLASS()
class DUNGEONS_API UItemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static FSerializableItemId MakeItemId(FSerializableItemId inId);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static FName BreakItemId(const FSerializableItemId& inId);

	UFUNCTION(BlueprintPure, meta=(DisplayName="Equal (ItemTypeID)", CompactNodeTitle="=="), Category="Items|ItemTypeID")
	static bool EqualEqual_ItemTypeID(const FSerializableItemId& A, const FSerializableItemId& B );
	
	UFUNCTION(BlueprintPure, meta=(DisplayName="NotEqual (ItemTypeID)", CompactNodeTitle="!="), Category="Items|ItemTypeID")
	static bool NotEqual_ItemTypeID(const FSerializableItemId& A, const FSerializableItemId& B );

	UFUNCTION(BlueprintPure, BlueprintPure, Category = "Items|ItemTypeID")
	static bool GetIsUniqueForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static UTexture2D* GetIconTextureForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static UTexture2D* GetIconTextureForItemTypeHigh(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Dungeons")
	static UTexture2D* StreamIconTextureForItemIdHigh(const FSerializableItemId& type, bool blockingLoad = false);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static UTexture2D* GetAmmoIconSmallTextureForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static UTexture2D* GetGearIconTextureForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static bool GetIsSoulUseForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static bool GetIsSoulGatherForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static ESlotType GetSlotTypeForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static bool GetIsGearForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static FText GetNameForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static FText GetDescriptionForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static float GetDefaultDurationForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static float GetDefaultCooldownForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static bool GetHasCooldownForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static ItemTag GetTagForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static bool DoesSlotTypeAcceptItemType(ESlotType comparedSlotType, const FSerializableItemId& comparedItemType);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static bool GetIsInventoryOnlyForItemType(const FSerializableItemId& type);
	
	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static bool GetIsPassiveForItemType(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, Category = "Items|ItemTypeID")
	static bool IsItemIdValid(const FSerializableItemId& type);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "ItemTypeID To String", CompactNodeTitle = "->", BlueprintAutocast), Category = "Items|ItemTypeID")
	static FString Conv_ItemTypeIDToString(const FSerializableItemId& type);

	//preload gear icon textures, this could possibly be made internal to the item asset finder later
	static void PreloadGearIconTextures();
	static void ClearPreloadedGearIconTextures();

private:

	static UTexture2D* GetTextureHelper(const TOptional<FSoftObjectPath>);

	static UTexture2D* GetPreloadGearIconTextureInternal(const FSerializableItemId& type);

	static TArray< UTexture2D* >			s_PreloadGearIconTextures;
};