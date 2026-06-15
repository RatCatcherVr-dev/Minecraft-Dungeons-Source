#include "Dungeons.h"
#include "MerchantActorUtil.h"
#include "game/actor/character/merchant/MerchantActor.h"
#include "game/merchant/transaction/OfferHyperMissionOfferings.h"
#include <GameFramework/Character.h>
#include <EngineUtils.h>

AMerchantActor* UMerchantActorUtil::GetFirstSelectMissionOfferingsMerchant(ACharacter* interacter, ELevelNames level) {
	auto isOfferingsMerchant = [level](const AMerchantBase& merchant) -> bool {
		if (auto transaction = merchant.GetTransaction<UOfferHyperMissionOfferings>()) {
			return transaction->GetMission() == level;
		}
		return false;
	};
	
	for (auto merchant : TActorRange<AMerchantActor>(interacter->GetWorld())) {
		if (merchant->IsMerchantForMatching(interacter, isOfferingsMerchant)) {
			return merchant;
		}
	}
	return nullptr;
}