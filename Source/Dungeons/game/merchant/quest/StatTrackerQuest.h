#pragma once
#include "MerchantCountQuestBase.h"
#include "StatTracker.h"
#include "StatTrackerQuest.generated.h"

class AMerchantBase;

UCLASS()
class DUNGEONS_API UStatTrackerQuest : public UMerchantCountQuestBase {
	GENERATED_BODY()

	typedef std::function<int(const TrackedStats& stats)> StatFunction;
	StatFunction mStatFunction;
	FText mQuestLabel;
protected:
	int GetProgressCount() const override;
public:
	static UStatTrackerQuest* CreateSubobject(AMerchantBase* object, const FName& name, const FText& questLabel, StatFunction func, int requiredCount);
	FText GetProgressText() const override;
};
