#pragma once
#include "MerchantCountQuestBase.h"
#include "ProgressStatQuest.generated.h"

class AMerchantBase;

UCLASS()
class DUNGEONS_API UProgressStatQuest : public UMerchantCountQuestBase {
	GENERATED_BODY()
	
protected:
	EProgressStat mProgressStat = EProgressStat::WIN_MISSIONS;
	int GetProgressCount() const override;
	virtual EProgressStat GetProgressStat() const;

public:
	static UProgressStatQuest* CreateSubobject(AMerchantBase* object, const FName& name, EProgressStat stat, int requiredCount);
	FText GetProgressText() const override;
	FText GetExplainerText() const override;
};
