#pragma once
#include "MerchantSelectionBase.h"
#include "SelectMerchantSlot.generated.h"

class UMerchantSlotBase;

UCLASS(BlueprintType)
class DUNGEONS_API USelectMerchantSlot : public UMerchantSelectionBase {
	GENERATED_BODY()

protected:
	UPROPERTY()
	UMerchantSlotBase* mSelectedSlot;	
public:
	void EnsureSelection() override;
	void ClearSelection() override;

	TArray<TSubclassOf<UMerchantTransactionBase>> GetAvailableTransactions() const override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMerchantSlotBase* GetMerchantSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<UMerchantSlotBase*> GetSelectableMerchantSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SelectMerchantSlot(UMerchantSlotBase* merchantSlot);

	bool CanSelectAny() const override;
	bool HasSelectedAny() const override;
};

