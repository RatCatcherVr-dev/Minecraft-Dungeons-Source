#pragma once
#include "MerchantCountQuestBase.h"
#include "ObjectiveTagQuest.generated.h"

class AMerchantBase;

UCLASS()
class DUNGEONS_API UObjectiveTagQuest : public UMerchantCountQuestBase {
	GENERATED_BODY()

	std::string mObjectiveTag;
	FText mObjectiveLabelText;
	FText mObjectiveExplainerText;
protected:
	int GetProgressCount() const override;
public:
	static UObjectiveTagQuest* CreateSubobject(AMerchantBase* merchant, const FName& name, const std::string& objectiveTag, const FText& objectiveLabel, const FText& mObjectiveExplainerText, int requiredCount);
	FText GetProgressText() const override;
	FText GetExplainerText() const override;
};