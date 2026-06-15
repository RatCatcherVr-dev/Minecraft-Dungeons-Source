#pragma once
#include "game/merchant/transaction/MerchantTransactionBase.h"
#include "game/merchant/MerchantSubobjectBase.h"
#include "MerchantSelectionBase.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMerchantSelectionChangedInternal, const UMerchantSelectionBase&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMerchantSelectionConfirmedInternal, UMerchantSelectionBase&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMerchantSelectionCancelledInternal, UMerchantSelectionBase&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMerchantSelectionChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMerchantSelectionSelectableChanged);

UCLASS(BlueprintType, Abstract)
class DUNGEONS_API UMerchantSelectionBase : public UMerchantSubobjectBase {
	GENERATED_BODY()
protected:
	void SelectionChanged() const;
	void SelectionSelectableChanged() const;

	void OnSetupSession() override;

public:
	virtual void Refresh() {};

	FOnMerchantSelectionChangedInternal OnSelectionChangedInternal;
	FOnMerchantSelectionChangedInternal OnSelectionChangedInternalAfterBlueprints;
	FOnMerchantSelectionConfirmedInternal OnSelectionConfirmedInternal;
	FOnMerchantSelectionCancelledInternal OnSelectionCancelledInternal;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMerchantSelectionChanged OnSelectionChanged;

	virtual TArray<TSubclassOf<UMerchantTransactionBase>> GetAvailableTransactions() const {
		return {};
	};

	virtual bool ShouldShowEnchantmentPoints() const { return false; }
	virtual void EnsureSelection() {};
	virtual void ClearSelection() {};

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool CanSelectAny() const { return false; };

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool HasSelectedAny() const { return false; };

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool TryCancelSelection();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool TryConfirmSelection();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanCancelSelection() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanConfirmSelection() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsFocused() const;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMerchantSelectionSelectableChanged OnMerchantSelectionSelectableChanged;
};

