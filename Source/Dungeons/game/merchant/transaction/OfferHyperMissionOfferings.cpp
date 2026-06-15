#include "Dungeons.h"
#include "util/Algo.h"
#include "game/item/ItemArchetypeCounts.h"
#include "game/merchant/MerchantSession.h"
#include "game/merchant/type/MerchantBase.h"
#include "game/merchant/slot/OfferedItemSlot.h"
#include "game/merchant/selection/OfferEnchantmentPoints.h"
#include "game/mission/dlc/EligibleDLC.h"
#include "game/mission/offerings/MissionOfferingsUtil.h"
#include "game/component/ItemStashComponent.h"
#include "game/component/MissionProgressComponent.h"
#include "game/util/ActorQuery.h"
#include "gamemodes/LobbyActor.h"
#include "OfferHyperMissionOfferings.h"


FMissionOfferings UOfferHyperMissionOfferings::CreateMissionOfferings() const {
	auto offerSlots = GetMerchant()->GetSlots<UOfferedItemSlot>();
	auto offeredEnchantmentPoints = GetMerchant()->GetSelection<UOfferEnchantmentPoints>();
	int offeredPoints = offeredEnchantmentPoints ? offeredEnchantmentPoints->GetAvailableOfferedEnchantmentPoints() : 0;
	return {
		algo::map_if_tarray(offerSlots, RETLAMBDA(it->GetInventoryItem()), RETLAMBDA(it)),
		offeredPoints,
	};
}

bool UOfferHyperMissionOfferings::IsItemBeingOffered(UInventoryItem* item) const {
	auto offerSlots = GetMerchant()->GetSlots<UOfferedItemSlot>();
	return algo::any_of(offerSlots, RETLAMBDA(it->GetInventoryItem() == item));
}

void UOfferHyperMissionOfferings::PopulateRequiredSelectionClasses(TSet<TSubclassOf<UMerchantSelectionBase>>& selections) const {
	selections.Add(UOfferEnchantmentPoints::StaticClass());		
}

FMerchantTransactionStatus UOfferHyperMissionOfferings::Validate() const {

	auto& session = GetSession();
	
	if (auto lobbyActor = actorquery::getFirstActor<ALobbyActor>(GetWorld())) {
		if (lobbyActor->IsRequestingLevel()) {
			return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::MISSION_VOTE_IN_PROGRESS) };
		}
	}

	auto mission = GetMission();
	auto& missiondef = missions::get(mission);

	//this check is more of a sanity check based on our current design - it should never trigger unless we make a mistake
	if (!missiondef.isHyperMission()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::NOT_A_HYPERMISSION) };
	}

	if (missiondef.isDisabled()) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::MISSION_DISABLED) };
	}	

	auto offerings = CreateMissionOfferings();	
	if (algo::any_of(offerings.offeredItems, RETLAMBDA(it->IsNetherite()))) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CANNOT_OFFER_GILDED_ITEMS) };
	}

	if (algo::any_of(offerings.offeredItems, RETLAMBDA(it->IsCloned()))) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CANNOT_OFFER_CLONED_ITEMS) };
	}

	if (const auto& problem = UMissionOfferingsUtil::QueryMissionOfferingsProblem(GetMission(), offerings.getOfferingsSummary(), ItemArchetypeCounts(offerings.offeredItems), FEligibleDLC(session.GetMissionProgress()->GetOwnedDLCs(), offerings.offeredItems))) {
		return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::OFFERING_PROBLEM, problem.GetValue() ) };
	}

	return { FMerchantExecuteStatus(EMerchantTransactionStatusReason::CONFIRM_OFFERINGS), {}, {},
		FMerchantTransactionStatus::ValidatedTransaction([offerings, session, mission]() -> void {
			if (auto itemStash = session.GetItemStashComponentMutable()) {
				itemStash->SetMissionOfferings(mission, offerings);
			}
		})
	};
}

EMerchantTransactionStatusReason UOfferHyperMissionOfferings::SuccessReason() const {
	return EMerchantTransactionStatusReason::CONFIRM_OFFERINGS;
}

