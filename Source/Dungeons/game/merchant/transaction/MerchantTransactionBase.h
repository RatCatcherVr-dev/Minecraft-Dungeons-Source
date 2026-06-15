#pragma once
#include "game/merchant/MerchantContext.h"
#include "MerchantTransactionStatus.h"
#include "game/merchant/MerchantSubobjectBase.h"
#include "game/merchant/currency/MerchantPrice.h"
#include "ui/status/ProblemStatus.h"
#include "MerchantTransactionBase.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnMerchantTransactionExecutedInternal, const UMerchantTransactionBase&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMerchantTransactionChanged);

class UMerchantSelectionBase;

UCLASS(BlueprintType, Abstract)
class DUNGEONS_API UMerchantTransactionBase : public UMerchantSubobjectBase {
	GENERATED_BODY()
protected:
	virtual FMerchantTransactionStatus Validate() const {
		return {};
	}
	virtual void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {}	
	virtual EMerchantTransactionStatusReason SuccessReason() const { return EMerchantTransactionStatusReason::UNSET; }


	void TransactionChanged() const;	

public:
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EMerchantTransactionStatusReason GetSuccessStatusReason() const { return SuccessReason(); }

	void Refresh();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMerchantDisplayPrice GetPrice() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasPrice() const;

	FOnMerchantTransactionExecutedInternal OnMerchantTransactionExecutedInternal;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMerchantTransactionChanged OnMerchantTransactionChanged;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	EMerchantTransactionStatusReason GetStatusReason() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	TArray<FItemBulletPoint> GetStatusBulletPoints() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FProblemStatus QueryProblemStatus(bool& hasProblem) const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool TryExecute();

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanExecute() const;	

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool CanSelectAllRequiredSelections() const;		

	TSet<TSubclassOf<UMerchantSelectionBase>> GetRequiredSelectionClasses() const;
};

