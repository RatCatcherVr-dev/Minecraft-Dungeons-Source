#include "Dungeons.h"
#include "DynamicProgressStatQuest.h"
#include "game/merchant/type/MerchantBase.h"

UDynamicProgressStatQuest* UDynamicProgressStatQuest::CreateSubobject(AMerchantBase* merchant, const FName& name, EProgressStat fallbackStat, int requiredCount, ProgressStatProvider statProvider) {
	auto quest = merchant->CreateDefaultSubobject<UDynamicProgressStatQuest>(name);
	quest->mProgressStat = fallbackStat;
	quest->mProgressStatProvider = statProvider;
	quest->mCountToComplete = requiredCount;
	return quest;
}

void UDynamicProgressStatQuest::ResetProgress() const {
	EditOrCreateDynamicQuestState().setSelectedProgressStat(mProgressStatProvider());
	Super::ResetProgress();
}

EProgressStat UDynamicProgressStatQuest::GetProgressStat() const {	
	if (auto state = ReadDynamicQuestState()) {
		return state.GetValue().getSelectedProgressStat().Get(mProgressStat);
	}
	return Super::GetProgressStat();
}