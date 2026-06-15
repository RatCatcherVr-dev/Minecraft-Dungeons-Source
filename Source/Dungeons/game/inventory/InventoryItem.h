#pragma once

#include "CoreMinimal.h"
#include "game/item/InventoryItemData.h"
#include "game/item/ItemBulletPoint.h"
#include "game/item/ItemCharacteristic.h"
#include "game/item/ItemType.h"
#include "InventoryItem.generated.h"

class UItemStashComponent;
class ABaseCharacter;

DECLARE_MULTICAST_DELEGATE(FItemChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEnchantmentChanged, int32, enchantmentIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemMetaChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemEnchanted, bool, success, int32, enchantmentIndex);

UCLASS(BlueprintType)
class DUNGEONS_API UInventoryItem : public UObject {
	GENERATED_BODY()
public:
	static const float DIAMOND_DUST_UPGRADE_AMOUNT;

	UPROPERTY(BlueprintReadOnly)
	FInventoryItemData Item;
	UFUNCTION(BlueprintCallable)
	bool EnchantIndex(int32 index);
	UFUNCTION(BlueprintCallable)
    bool ReplaceEnchantment(const int32 index, const FEnchantmentData& enchantment);
	UFUNCTION(BlueprintCallable)
	bool CanEnchantIndex(int32 index) const;
	UFUNCTION(BlueprintCallable)
	bool IsEnchantIndexLocked(int32 index) const;
	UFUNCTION(BlueprintCallable)
	bool IsEnchantIndexDisabled(int32 index) const;
	UFUNCTION(BlueprintCallable)
	bool IsEnchantIndexMaxed(int32 index) const;
	UFUNCTION(BlueprintCallable)
	bool IsEnchantIndexAffordable(int32 index) const;
	UFUNCTION(BlueprintCallable)
	bool IsEnchantIndexEnchanted(int32 index) const;
	UFUNCTION(BlueprintCallable)
	bool IsEnchantIndexHidden(int32 index) const;
	UFUNCTION(BlueprintCallable)
	int GetTotalInvestedEnchantmentPoints() const;
	UFUNCTION(BlueprintCallable)
	int GetTotalEnchantmentRows() const;
	UFUNCTION(BlueprintCallable)
	bool Enchantable() const;
	UFUNCTION(BlueprintCallable)
	bool IsGear() const;
	UFUNCTION(BlueprintCallable)
	bool IsGifted() const;
	UFUNCTION(BlueprintCallable)
	bool IsNetherite() const;
	UFUNCTION(BlueprintCallable)
	bool HasTag(ItemTag tag) const;	
	UFUNCTION(BlueprintCallable)
	bool IsCloned() const;
	UFUNCTION(BlueprintCallable)
	ItemTag GetTag() const;
	UFUNCTION(BlueprintCallable)
	bool IsInventoryOnly() const;
	
	UFUNCTION(BlueprintCallable)
	bool CanBeUpgraded() const;
	UFUNCTION(BlueprintCallable)
	bool GetIsUpgraded() const;
	UFUNCTION(BlueprintCallable)
	bool TryUpgradeItem();

	UFUNCTION(BlueprintCallable)
	bool CanSalvage() const;

	UFUNCTION(BlueprintCallable)
	bool CanEnchant() const;

	UPROPERTY(BlueprintReadOnly)
	FInventoryItemMetaData Meta;

	const FInventoryItemData& GetItemData() const;

	bool IsNew() const;

	UFUNCTION(BlueprintCallable)
	const FText& GetDisplayNameText() const;

	UFUNCTION(BlueprintCallable)
	const FText& GetDisplayDescriptionText() const;

	UFUNCTION(BlueprintCallable)
	const FText& GetDisplayFlavourText() const;

	UFUNCTION(BlueprintCallable)
	const TArray<FItemCharacteristic>& GetDisplayCharacteristicsText();

	UFUNCTION(BlueprintCallable)
	const TArray<FItemBulletPoint>& GetDisplayBulletPointsText(ABaseCharacter* owner);
	
	UFUNCTION(BlueprintCallable)
	FText GetDisplayItemPowerText() const;

	UFUNCTION(BlueprintCallable)
	int GetDisplayItemPowerInt() const;
	
	void SetItemPower(float power);
	float GetItemPower() const;

	UFUNCTION(BlueprintCallable)
	bool IsMarkedNew() const;
	UFUNCTION(BlueprintCallable)
	void ClearMarkedNew();

	UFUNCTION(BlueprintCallable)
	bool IsBeingOffered() const;

	UPROPERTY(BlueprintAssignable)
	FOnItemChanged OnItemChanged;

	UPROPERTY(BlueprintAssignable)
	FOnItemEnchantmentChanged OnItemEnchantmentChanged;

	UPROPERTY(BlueprintAssignable)
	FOnItemMetaChanged OnMetaChanged;

	UPROPERTY(BlueprintAssignable)
	FOnItemEnchanted OnItemEnchanted;

	//vrak: Renamed it since OnItemChanged is now used in blueprints, but I dont
	//want blueprints to be able to remove binds to this internal delegate since they are used in the internal
	//class hiarchy for equipment slots.
	FItemChanged OnItemChangedInternal; 

private:
	UPROPERTY()
	TArray<FItemCharacteristic> DisplayCharacteristics;

	UPROPERTY()
	TArray<FItemBulletPoint> DisplayBulletPoints;

	UPROPERTY()
	TArray<FText> DisplayPowerEffects;

	class UItemStashComponent* ItemStashComponent = nullptr;
	friend class UItemStashComponent;
};
