#include "Dungeons.h"
#include "game/merchant/selection/SelectMerchantSlot.h"
#include "QuestGiverMerchant.h"

AQuestGiverMerchant::AQuestGiverMerchant(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	mPermanentSelectionClass = USelectMerchantSlot::StaticClass();
}
