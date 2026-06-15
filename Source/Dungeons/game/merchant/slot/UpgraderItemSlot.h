#pragma once
#include "MerchantItemSlotBase.h"
#include "UpgraderItemSlot.generated.h"

UCLASS()
class DUNGEONS_API UUpgraderItemSlot : public UMerchantItemSlotBase {
	GENERATED_BODY()

private:
	UPROPERTY()
	UMerchantQuestBase* mUpgradeQuest;	

	bool IsUpgradeCompleted() const;
protected:

	void OnItemSet() override;
		
	virtual UMerchantQuestBase* GetCurrentUpgradeQuest() const;
public:


	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMerchantQuestBase* GetActiveUpgradeQuest() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	UMerchantQuestBase* GetCompletedUpgradeQuest() const;

	bool ShouldShowObjectiveMarker() const override;

	static UUpgraderItemSlot* CreateSubobject(UObject* object, const FName& name, UMerchantQuestBase* upgradeQuest = nullptr);
	UUpgraderItemSlot& SetUpgradeQuest(UMerchantQuestBase* quest);

	bool IsItemUnlocked() const override;

	TArray<TSubclassOf<UMerchantTransactionBase>> GetAvailableTransactions() const override;

	bool CanRestock() const override;

	void EnsureSaveData() override;
};