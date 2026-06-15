#include "Dungeons.h"
#include "ObjectiveTagQuest.h"
#include "game/util/ValueFormat.h"
#include "game/merchant/type/MerchantBase.h"



UObjectiveTagQuest* UObjectiveTagQuest::CreateSubobject(AMerchantBase* merchant, const FName& name, const std::string& objectiveTag, const FText& objectiveLabel, const FText& mObjectiveExplainerText, int requiredCount) {
	auto quest = merchant->CreateDefaultSubobject<UObjectiveTagQuest>(name);
	quest->mObjectiveTag = objectiveTag;
	quest->mObjectiveLabelText = objectiveLabel;
	quest->mObjectiveExplainerText = mObjectiveExplainerText;
	quest->mCountToComplete = requiredCount;
	return quest;
}

FText UObjectiveTagQuest::GetProgressText() const {
	return mObjectiveLabelText;
}

FText UObjectiveTagQuest::GetExplainerText() const {
	return FText::Format(mObjectiveExplainerText, GetDisplayTargetCount());
}

int UObjectiveTagQuest::GetProgressCount() const {
	return GetContext().GetFinishedObjectiveTagCount(mObjectiveTag);
}