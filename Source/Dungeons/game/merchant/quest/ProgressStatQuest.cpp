#include "Dungeons.h"
#include "game/progress/ProgressStat.h"
#include "game/util/ValueFormat.h"
#include "game/merchant/type/MerchantBase.h"
#include "ProgressStatQuest.h"

UProgressStatQuest* UProgressStatQuest::CreateSubobject(AMerchantBase* merchant, const FName& name, EProgressStat stat, int requiredCount) {
	auto quest = merchant->CreateDefaultSubobject<UProgressStatQuest>(name);
	quest->mProgressStat = stat;
	quest->mCountToComplete = requiredCount;
	return quest;
}

int UProgressStatQuest::GetProgressCount() const {
	return GetContext().GetProgressStat(GetProgressStat());
}

EProgressStat UProgressStatQuest::GetProgressStat() const {
	return mProgressStat;
}

FText UProgressStatQuest::GetProgressText() const {
	return UProgressStatUtil::GetProgressStatText(GetProgressStat());
}

FText UProgressStatQuest::GetExplainerText() const {
	return UProgressStatUtil::GetProgressStatExplainerText(GetProgressStat(), GetDisplayTargetCount());
}
