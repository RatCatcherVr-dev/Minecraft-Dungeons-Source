#pragma once
#include "game/merchant/transaction/MerchantTransactionBase.h"
#include <Components/ActorComponent.h>
#include "game/merchant/MerchantContext.h"
#include "game/merchant/quest/MerchantQuestBase.h"
#include "game/merchant/MerchantSaveData.h"
#include "game/merchant/MerchantSubobjectBase.h"
#include "game/merchant/currency/MerchantPrice.h"
#include "game/merchant/selection/MerchantSelectionBase.h"
#include "MerchantSlotBase.generated.h"

class AMerchantBase;

DECLARE_MULTICAST_DELEGATE(FOnMerchantSlotChangedInternal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMerchantSlotChanged);

UCLASS(BlueprintType, Abstract)
class DUNGEONS_API UMerchantSlotBase : public UMerchantSubobjectBase {
	GENERATED_BODY()

protected:
	UPROPERTY()
	UMerchantQuestBase* mUnlockQuest;

	virtual void WasChanged() const;	

	virtual void PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {}
public:	
	void SetUnlockQuest(UMerchantQuestBase*);

	bool HasCompletedUnlockQuest() const;

	virtual bool ShouldShowObjectiveMarker() const;
	
	FOnMerchantSlotChangedInternal OnMerchantSlotChangedInternal;

	UPROPERTY(BlueprintAssignable, Category = "Dungeons")
	FOnMerchantSlotChanged OnMerchantSlotChanged;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMerchantQuestBase* GetActiveUnlockQuest() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool IsUnlocked() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	bool HasPrice() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	FMerchantDisplayPrice GetPrice() const;

	virtual void Restock() {};

	virtual bool CanRestock() const { return false; };

	TSet<TSubclassOf<UMerchantSelectionBase>> GetRequiredSelectionClasses() const;

	virtual TArray<TSubclassOf<UMerchantTransactionBase>> GetAvailableTransactions() const {
		return {};
	}

	virtual TOptional<FMerchantPricing> GetOptionalPrice() const {
		return {};
	}	

	const FMerchantSlotSaveData& ReadSaveData() const;
	FMerchantSlotSaveData& EditSaveData() const;
	virtual void EnsureSaveData();
};