#include "Dungeons.h"
#include "MissionOfferingsUtil.h"
#include "OfferingsSummary.h"
#include "game/component/ItemStashComponent.h"
#include "game/item/ItemArchetypeCounts.h"
#include "game/inventory/InventoryItemUtil.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/MissionDLCUtil.h"
#include "game/mission/dlc/EligibleDLC.h"
#include "game/mission/hyper/HyperMissions.h"
#include "game/mission/hyper/DungeonEligibility.h"
#include "game/mission/state/MissionState.h"
#include "game/util/ValueFormat.h"
#include "util/Algo.hpp"




TMap<EItemArchetype, int> UMissionOfferingsUtil::GetOfferingsArchetypeCounts(const FMissionOfferings& offerings) {
	return UInventoryItemUtil::GetItemsArchetypeCounts(algo::map_tarray(offerings.offeredItems, RETLAMBDA(it->Item)));
}

TOptional<FProblemStatus> UMissionOfferingsUtil::QueryMissionOfferingsProblem(ELevelNames levelName, const FOfferingsSummary& offerings, const ItemArchetypeCounts& archetypeCounts, const FEligibleDLC& eligibleDLC) {
	const auto& missionDef = missions::get(levelName);

	{
		auto requiredCount = missionDef.getRequiredOfferedItemCount();
		if (requiredCount.IsSet() && offerings.offeredCount < requiredCount.GetValue()) {
			if (requiredCount.GetValue() == 1) {
				return FProblemStatus{
					NSLOCTEXT("MissionProgress", "AnyOfferingRequired", "Add Offering"),
					NSLOCTEXT("MissionProgress", "MustOfferAtLeastOneItem", "Offering must contain at least one item")
				};
			}
			else {
				return FProblemStatus{
					NSLOCTEXT("MissionProgress", "MoreOfferingsRequired", "Add Offerings"),
					FText::Format(NSLOCTEXT("MissionProgress", "MustOfferXItems", "Must offer {0} items"), valueformat::getAsWordOrNumber(requiredCount.GetValue())),
					valueformat::asCounter(offerings.offeredCount, requiredCount.GetValue())
				};
			}
		}
	}

	{
		auto requiredDisplayItemPower = missionDef.getRequiredOfferedDisplayItemPower();	
		if (requiredDisplayItemPower.IsSet() && offerings.offeredTotalPower < requiredDisplayItemPower.GetValue()) {
			return FProblemStatus{
				NSLOCTEXT("MissionProgress", "OfferingsTooWeak", "Offering Too Weak"),
				FText::Format(NSLOCTEXT("MissionProgress", "OfferingTotalItemPowerMustBeAtLeastX", "Total item power must be at least {0}"), valueformat::asNumber(requiredDisplayItemPower.GetValue())),
				valueformat::asCounter(offerings.offeredTotalPower, requiredDisplayItemPower.GetValue())
			};
		}
	}

	{
		auto requiredNumAncientMobTypes = missionDef.getRequiredNumAncientMobTypes();
		if (requiredNumAncientMobTypes.IsSet()) {
			if (missionDef.countNumAncientMobTypes(missions::hajper::getHyperDungeonLevelGenerationPredicate(eligibleDLC, archetypeCounts)) < requiredNumAncientMobTypes.GetValue()) {
				return FProblemStatus{
					NSLOCTEXT("MissionProgress", "NoAncients", "No Ancients"),
					NSLOCTEXT("MissionProgress", "NoAncientMobsRespondToTheseOfferings", "No Ancient mobs respond to these offerings"),
				};
			}
		}
	}
	
	return {};
}

TOptional<FProblemStatus> UMissionOfferingsUtil::QueryMissionOfferingsProblem(const FMissionState& state) {
	return QueryMissionOfferingsProblem(state.getLevelName(), state.getOfferingsSummary(), state.getItemArchetypeCounts(), state.getEligibleDLCs());
}

bool UMissionOfferingsUtil::EqualEqual_MissionOfferings(const FMissionOfferings& A, const FMissionOfferings& B) {
	return A == B;
}

bool UMissionOfferingsUtil::NotEqual_MissionOfferings(const FMissionOfferings& A, const FMissionOfferings& B) {
	return !(A == B);
}
