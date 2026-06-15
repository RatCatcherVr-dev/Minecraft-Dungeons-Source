#pragma once
#include "MerchantQuestBase.h"
#include "game/merchant/MerchantSaveData.h"
#include "MerchantCountQuestBase.generated.h"

UCLASS(Abstract)
class DUNGEONS_API UMerchantCountQuestBase : public UMerchantQuestBase {
	GENERATED_BODY()
protected:
	int mCountToComplete = 1;

	virtual int GetProgressCount() const { return 0; };	

	int GetDisplayCount() const;
	int GetDisplayTargetCount() const;
public:
	bool IsCompleted() const override;
	int GetCompletedCount() const override;
	float GetProgressFraction() const override;		

	void InitSaveData(FMerchantQuestSaveData& saveData) const override;	
	TOptional<FCountQuestState> ReadCountQuestState() const;

	FText GetCountText() const override;
};