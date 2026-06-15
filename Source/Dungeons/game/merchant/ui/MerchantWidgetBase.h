#pragma once
#include <UserWidget.h>
#include "game/merchant/type/MerchantBase.h"
#include "DungeonsWidgets/DungeonsUserWidget.h"
#include "game/merchant/announcement/ItemAnnouncement.h"
#include "MerchantWidgetBase.generated.h"

class UMerchantSlotBase;
class UInventoryItem;
class UMerchantTransactionBase;
class UMerchantCurrencyComponent;

UCLASS()
class DUNGEONS_API UMerchantWidgetBase : public UUserWidget {
	GENERATED_BODY()
private:
	UPROPERTY()
	AMerchantBase* mMerchant = nullptr;

	void OnRequiredSelectionsChanged();
	void OnAvailableTransactionsChanged();
	void OnSelectionFocusChanged();
	void OnMerchantTransactionExecuted(const UMerchantTransactionBase& transaction);
public:
	void BindTo(AMerchantBase* merchant);
	bool IsBoundTo(AMerchantBase* merchant) const;

protected:	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSelections();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateTransactions();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSelectionFocus();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSlots();

	UFUNCTION(BlueprintImplementableEvent)
	void OnBoundMerchant(AMerchantBase* merchant);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnboundMerchant(AMerchantBase* merchant);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBindChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void OnItemAnnouncement(const FItemAnnouncement& itemAnnouncement);

	UFUNCTION(BlueprintImplementableEvent)
	void OnTransactionExecuted(const UMerchantTransactionBase* transaction);

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsTransactionClassAvailable(const TSubclassOf<UMerchantTransactionBase>& transactionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsSelectionClassRequired(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsSelectionClassFocused(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	USelectionChain* GetSelectionFocus() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMerchantSelectionBase* GetSelectionByClass(const TSubclassOf<UMerchantSelectionBase>& selectionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMerchantTransactionBase* GetTransactionByClass(const TSubclassOf<UMerchantTransactionBase>& transactionClass) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<UMerchantSlotBase*> GetSlots() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<UMerchantSlotBase*> GetSlotsByClass(const TSubclassOf<UMerchantSlotBase>& slotClass) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetDisplayName() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetDisplayDescription() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<FItemBulletPoint> GetDisplayBulletPoints() const;
	
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	const FText& GetLockedSlotsHint() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasAnyLockedSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool ShouldShowEnchantmentPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<FItemBulletPoint> GetRestockMethodsBulletPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	AMerchantActor* GetMerchantActorOwner() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMerchantCurrencyComponent* GetCurrencyComponent() const;
};
