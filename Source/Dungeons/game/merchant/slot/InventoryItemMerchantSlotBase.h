#pragma once
#include "game/merchant/selection/SelectInventorySlotItem.h"
#include "MerchantSelectionSlotBase.h"
#include "InventoryItemMerchantSlotBase.generated.h"

class UInventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryItemMerchantSlotChanged);

UCLASS(BlueprintType)
class DUNGEONS_API UInventoryItemMerchantSlotBase : public UMerchantSelectionSlotBase {
	GENERATED_BODY()
private:
	UPROPERTY()
	UInventoryItem* mInventoryItem = nullptr;
	
	void ValidateSelectedInventoryItem();
protected:

	void WasChanged() const override;

	void OnSetupSession() override;
	void OnCleanupSession() override;

public:
	
	void SetInventoryItem(UInventoryItem*);
	void ClearInventoryItem();
	bool HasInventoryItem() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnInventoryItemMerchantSlotChanged OnInventoryItemMerchantSlotChanged;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual UInventoryItem* GetInventoryItem() const;	
};
