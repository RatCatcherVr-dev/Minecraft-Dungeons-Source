#include "Dungeons.h"
#include "StatTrackerQuest.h"
#include "StatTracker.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/merchant/MerchantContext.h"
#include "game/util/ValueFormat.h"

UStatTrackerQuest* UStatTrackerQuest::CreateSubobject(AMerchantBase* merchant, const FName& name, const FText& questLabel, StatFunction statFunc, int requiredCount) {
	UStatTrackerQuest* quest = merchant->CreateDefaultSubobject<UStatTrackerQuest>(name);
	quest->mStatFunction = statFunc;
	quest->mQuestLabel = questLabel;
	quest->mCountToComplete = requiredCount;
	return quest;
}

int UStatTrackerQuest::GetProgressCount() const {
	//auto stats = GetContext().GetShopper().GetStatTracker()->GetStats();
	//return mStatFunction(stats);
	return 0;
}

FText UStatTrackerQuest::GetProgressText() const {
	return valueformat::asCounterLabeled(mQuestLabel, GetDisplayCount(), GetDisplayTargetCount());
}
