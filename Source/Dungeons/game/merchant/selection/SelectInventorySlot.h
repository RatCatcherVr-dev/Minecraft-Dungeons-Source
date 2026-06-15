#pragma once
#include "MerchantSelectionBase.h"
#include "game/item/ItemTypeDefs.h"
#include "SelectInventorySlot.generated.h"

class UInventoryItemSlot;

UCLASS(BlueprintType)
class DUNGEONS_API USelectInventorySlot : public UMerchantSelectionBase {
	GENERATED_BODY()

private:
	int mLastChangeIndex = 0;

	void RefreshSelectionCache();

	UPROPERTY(Transient)
	TArray<UInventoryItemSlot*> SelectionCache;

protected:
	ESlotType mUISlotTypeIcon = ESlotType::Any;

	UPROPERTY()
	UInventoryItemSlot* mSelectedInventorySlot;

	void OnSetupSession() override;
	void OnCleanupSession() override;

	virtual bool IsInventorySlotSelectable(UInventoryItemSlot* slot) const;
public:
	void EnsureSelection() override;
	void ClearSelection() override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UInventoryItemSlot* GetInventorySlot() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const TArray<UInventoryItemSlot*>& GetSelectableInventorySlots() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	void SelectInventorySlot(UInventoryItemSlot* slot);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool PollHasSelectablesChanged();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	ESlotType GetUISlotTypeIcon() const;

	bool CanSelectAny() const override;
	bool HasSelectedAny() const override;
};