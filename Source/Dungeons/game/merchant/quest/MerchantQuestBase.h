#pragma once
#include "game/merchant/MerchantContext.h"
#include "game/merchant/MerchantSaveData.h"
#include "game/merchant/MerchantSubobjectBase.h"
#include "MerchantQuestBase.generated.h"

UCLASS(Abstract)
class DUNGEONS_API UMerchantQuestBase : public UMerchantSubobjectBase {
	GENERATED_BODY()
protected:
	virtual void InitSaveData(FMerchantQuestSaveData& saveData) const {};
	const FMerchantQuestSaveData& ReadSaveData() const;
	FMerchantQuestSaveData& EditSaveData() const;

public:
	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual bool IsCompleted() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual int GetCompletedCount() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual float GetProgressFraction() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual FText GetCountText() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual FText GetProgressText() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	virtual FText GetExplainerText() const;
	
	bool HasSaveData() const;
	bool EnsureSaveData() const;
	virtual void ResetProgress() const;

	const TOptional<FDynamicQuestState>& ReadDynamicQuestState() const;
	FDynamicQuestState& EditOrCreateDynamicQuestState() const;
};