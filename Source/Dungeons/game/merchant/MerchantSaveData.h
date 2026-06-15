#pragma once

#include "game/item/InventoryItemData.h"
#include "game/progress/ProgressStat.h"
#include "MerchantSaveData.generated.h"

USTRUCT()
struct DUNGEONS_API FMerchantPricingSaveData {
	GENERATED_BODY()

	int mTimesRestocked = 0;
	void IncrementRestocked();
	int GetTimesRestocked() const;
	void ResetTimesRestocked();
};

USTRUCT()
struct DUNGEONS_API FMerchantSlotSaveData {
	GENERATED_BODY()
	
	TOptional<FInventoryItemData> item;
	float mPriceMultiplier = 1.0f;
	float mRebateFraction = 0.0f;
	bool bReserved = false;
};

USTRUCT()
struct FCountQuestState {
	GENERATED_BODY()
	int mTargetCount;
	int mStartedAtCount;

	bool IsCompleted(int count) const;
	float GetFraction(int count) const;
	int GetDeltaCountClamped(int count) const;
	int GetDeltaCount(int count) const;
	int GetDeltaTargetCount() const;
};

USTRUCT()
struct FDynamicQuestState {
	GENERATED_BODY()
	TOptional<EProgressStat> mSelectedProgressStat;

	TOptional<EProgressStat> getSelectedProgressStat() const;
	void setSelectedProgressStat(EProgressStat);
};

USTRUCT()
struct DUNGEONS_API FMerchantQuestSaveData {
	GENERATED_BODY()

	TOptional<FCountQuestState> mCountQuestState;
	TOptional<FDynamicQuestState> mDynamicQuestState;

	bool HasDynamicState() const;
	const TOptional<FDynamicQuestState>& ReadDynamicState() const;
	FDynamicQuestState& CreateDynamicState();
	FDynamicQuestState& EditDynamicState();
};

USTRUCT()
struct DUNGEONS_API FMerchantSaveData {
	GENERATED_BODY()

	bool mEverInteracted = false;
	TMap<FName, FMerchantSlotSaveData> mSlots;
	TMap<FName, FMerchantQuestSaveData> mQuests;
	FMerchantPricingSaveData mPricing;

	bool HasInteracted() const;
	void SetInteracted(bool interacted);

	bool HasSlot(const FName& id) const;
	void CreateSlot(const FName& id);
	const FMerchantSlotSaveData& ReadSlot(const FName& id) const;
	FMerchantSlotSaveData& EditSlot(const FName& id);

	bool HasQuest(const FName& id) const;
	FMerchantQuestSaveData& CreateQuest(const FName& id);
	const FMerchantQuestSaveData& ReadQuest(const FName& id) const;
	FMerchantQuestSaveData& EditQuest(const FName& id);

	const FMerchantPricingSaveData& ReadPricing() const;
	FMerchantPricingSaveData& EditPricing();
};
