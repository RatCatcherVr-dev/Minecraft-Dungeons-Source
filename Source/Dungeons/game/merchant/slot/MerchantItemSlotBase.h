#pragma once
#include "MerchantSlotBase.h"
#include "game/item/InventoryItemData.h"
#include "game/merchant/MerchantContext.h"
#include "game/merchant/restock/MerchantItemGeneratorBase.h"
#include "MerchantItemSlotBase.generated.h"

class UMerchantItemGeneratorBase;
class UInventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMerchantItemSlotChanged);

UCLASS(BlueprintType)
class DUNGEONS_API UMerchantItemSlotBase : public UMerchantSlotBase {
	GENERATED_BODY()
private:
	UPROPERTY()
	UInventoryItem* mDisplayItemCache = nullptr;
	UPROPERTY()
	UInventoryItem* mDeliveredDisplayItem = nullptr;
	void RefreshDisplayItemCache();
protected:
	UPROPERTY()
	UMerchantItemGeneratorBase* mItemGenerator;

	float GetRebateFraction() const;
	void SetRebateFraction(float fraction) const;
	float GetPriceMultiplier() const;
	void SetPriceMultiplier(float multiplier) const;

	void WasChanged() const override;

	virtual void OnItemSet();
public:
	UMerchantItemSlotBase* SetItemGenerator(UMerchantItemGeneratorBase* itemGenerator);

	TOptional<FInventoryItemData> TryPopItem();
	FInventoryItemData PopItem(); //will crash if there is no item
	
	const TOptional<FInventoryItemData>& GetOptionalItem() const;

	void SetItem(FInventoryItemData itemToSet);
	void SetMerchantItem(FMerchantItemData generatedData);
	void ClearItem();
	void SetReserved(bool Reserved);

	void Restock() override;

	bool CanRestock() const override;

	void SetDeliveredDisplayItem(UInventoryItem*);

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMerchantItemSlotChanged OnMerchantItemSlotChanged;


	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FInventoryItemData& GetItem() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UInventoryItem* GetDisplayItemCache() const;	

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool HasItem() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool IsItemReserved() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool IsReserved() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool IsItemUnlocked() const;

	void EnsureSaveData() override;
};
