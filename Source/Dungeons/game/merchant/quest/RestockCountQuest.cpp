#include "Dungeons.h"
#include "game/merchant/type/MerchantBase.h"
#include "RestockCountQuest.h"


int URestockCountQuest::GetProgressCount() const {
	return GetContext().ReadSaveData().ReadPricing().GetTimesRestocked();
}

URestockCountQuest* URestockCountQuest::CreateSubobject(AMerchantBase* merchant, const FName& name) {
	auto quest = merchant->CreateDefaultSubobject<URestockCountQuest>(name);
	return quest;
}

