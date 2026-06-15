#include "Dungeons.h"
#include <Anticheat.hpp>
#include "game/component/ItemStashComponent.h"
#include "game/inventory/InventoryItemUtil.h"
#include "game/mission/MissionDLCUtil.h"
#include "util/Algo.h"
#include "MissionState.h"


namespace game { namespace  missionstate {
	FString GenerateStateGuid() {
		FGuid guid;
		FGenericPlatformMisc::CreateGuid(guid);
		return guid.ToString();
	}
}}


FMissionState::FMissionState(const FMissionDifficulty& missionDifficulty, RandomSeed seed, TArray<FInventoryItemData> items, int points, TArray<EDLCName> dlcs)
	: missionDifficulty(missionDifficulty)
	, seed(seed)
	, offeredItems(std::move(items))
	, ownedDLCs(std::move(dlcs))
	, offeredEnchantmentPoints(points)
	, livesLost(0)
	, partsDiscovered(0)
	, guid(game::missionstate::GenerateStateGuid())
{}

FMissionState::FMissionState(const FMissionDifficulty& missionDifficulty, RandomSeed seed, const FMissionOfferings& missionOfferings, TArray<EDLCName> dlcs) 
	: FMissionState(missionDifficulty
		, seed
		, algo::map_tarray(missionOfferings.offeredItems, RETLAMBDA(it->Item)), missionOfferings.offeredEnchantmentPoints
		, std::move(dlcs))
{}

FMissionState::FMissionState(const FMissionDifficulty& missionDifficulty, RandomSeed seed)
	: FMissionState(missionDifficulty, seed, {}, 0, {})
{}

ANTICHEAT_NO_OPTIMIZATION_BEGIN

int FMissionState::getItemCount() const {
	ANTICHEAT_VIRT_BEGIN
	return offeredItems.Num();
	ANTICHEAT_VIRT_END
}

int FMissionState::getTotalEnchantmentPoints() const {
	ANTICHEAT_VIRT_BEGIN
	return offeredEnchantmentPoints;
	ANTICHEAT_VIRT_END
}

int FMissionState::getTotalDisplayItemPower() const {
	ANTICHEAT_VIRT_BEGIN
	return algo::sum(offeredItems, RETLAMBDA(UInventoryItemUtil::GetDisplayItemPowerInt(it)));
	ANTICHEAT_VIRT_END
}

FEligibleDLC FMissionState::getEligibleDLCs() const {
	ANTICHEAT_VIRT_BEGIN
	return FEligibleDLC(ownedDLCs, offeredItems);	
	ANTICHEAT_VIRT_END
}

ItemArchetypeCounts FMissionState::getItemArchetypeCounts() const {
	ANTICHEAT_VIRT_BEGIN
	return ItemArchetypeCounts(offeredItems);
	ANTICHEAT_VIRT_END
}

ANTICHEAT_NO_OPTIMIZATION_END



EGameDifficulty FMissionState::getDifficulty() const {
	return missionDifficulty.difficulty;
}

EThreatLevel FMissionState::getThreatLevel() const {
	return missionDifficulty.threatLevel;
}

FEndlessStruggle FMissionState::getEndlessStruggle() const {
	return missionDifficulty.endlessStruggle;
}

ELevelNames FMissionState::getLevelName() const {
	return missionDifficulty.mission;
}

RandomSeed FMissionState::getSeed() const {
	return seed;
}

FOfferingsSummary FMissionState::getOfferingsSummary() const {
	return { getItemCount(), getTotalDisplayItemPower(), getTotalEnchantmentPoints() };
}
