#include "Dungeons.h"
#include "KillGroup.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/actor/item/ItemDropChanceActor.h"
#include "game/actor/item/ItemDropActor.h"
#include "game/level/GameTile.h"
#include "game/mobspawn/MobAction.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/mobspawn/SpawnRegions.h"
#include "game/objective/TargetLocators.h"
#include "game/objective/ObjectiveDoorLocker.h"
#include "game/objective/ObjectiveUtil.h"
#include "game/util/ActorQuery.h"
#include "util/StringUtil.h"
#include "game/GameBP.h"

namespace game { namespace objective {

KillGroup::KillGroup(const io::ObjectiveKillGroupData& data)
	: 
	mData(data)
{
	mKillGroupMobSpawnData = MakeShareable(new FSharedKillGroupMobSpawndata);

	for (auto&& group : mData.mobs.groups) {
		mobspawn::prepareMobGroup(group);
	}
	if (mData.showCount) {
		markPartiallyCompleted(0, mData.mobs.count);
	}
}

Validation KillGroup::validate(ValidationType) const {
	const bool stretchBased = !mData.stretch.empty();
	const bool valid = mTargetTile || !stretchBased;
	return validationErrorsIf({
		{ stretchBased && !mTargetTile, "Couldn't find tile for stretch: " + mData.stretch },
		{ valid && getSpawnRegions().isEmpty(), "Didn't find any spawn regions" },
		{ valid && !mDoorLocker, "Couldn't create door locker for blueprint: " + stringutil::toStdString(mData.gate.object.Get("default")) },
		{ mDoorLocker && !mDoorLocker->couldPlaceAllDoors(), "Couldn't place all doors" },
		{ mData.amount <= 0, std::string("Bad reward item set up - too low amount: ") + std::to_string(mData.amount) }, //D11.PS added std::string
		{ !GetItemRegistry().Request(stringutil::toFName(mData.rewardId )).IsSet(), "Bad reward item set up - ItemType from string is unset: " + mData.rewardId },
		{ mData.mobs.groups.empty(), "No mob group selected, can't set up group to kill" },
	});
}

FObjectiveLocations KillGroup::getLocations() const {
	return mMarkerLocations;
}

void KillGroup::onInit() {
	mTargetTile = getTargetTile();

	mMarkerLocations = toObjectiveLocations(targetFinder(), mData.markerRegion);

	mDoorLocker = objectiveDoorLocker(
		game(),
		mData.gate,
		mTargetTile ? doorsFromTileExits(game().tiles(), *mTargetTile) : DoorVector{},
		io::ObjectiveGate { io::ObjectiveGate::Nothing }
	);

	if (mDoorLocker) {
		mDoorLocker->lock();
	}
}

void KillGroup::onStart() {
	const auto config = mobspawn::configs::DefaultTileDataNoVariants(game()).Action(mobspawn::LongOffensiveRange());
	const auto spawnRegions = getSpawnRegions();
	const auto transformProvider = mobspawn::providers::FromProviders(mobspawn::providers::position::Regions(spawnRegions));

	AGameBP* gameBP = actorquery::getFirstActor<AGameBP>(&game().world());

	TSharedPtr<FSharedKillGroupMobSpawndata> capturedSpawnData = mKillGroupMobSpawnData;
	
	for (auto group : mobspawn::calculateMobsWithEnchantments(mData.mobs.groups, mData.mobs.count, game().settings().difficultyStats)) 
	{
		mKillGroupMobSpawnData->mRequestedMobSpawns += group.count;
		gameBP->RequestMobGroupSpawn(group, transformProvider, config, [](TArray<AMobCharacter*>& Mobs) {}, [capturedSpawnData](AMobCharacter* pMob) {
			
			--capturedSpawnData->mRequestedMobSpawns;
			if (pMob)
			{
				capturedSpawnData->mMobTargetGroup.Push(pMob);
				capturedSpawnData->mLastValidMobPosition = pMob->GetActorLocation();
			}
		
		});
	}
}

void KillGroup::onTick() {
	mKillGroupMobSpawnData->mMobTargetGroup.RemoveAllSwap(RETLAMBDA(!actorquery::is::alive(it.Get())));

	if (mData.showCount) {
		const auto killed = mData.mobs.count - mKillGroupMobSpawnData->mMobTargetGroup.Num() - mKillGroupMobSpawnData->mRequestedMobSpawns;
		markPartiallyCompleted(killed, mData.mobs.count);
	}
	if (mKillGroupMobSpawnData->mRequestedMobSpawns == 0)
	{
		if (mKillGroupMobSpawnData->mMobTargetGroup.Num() > 0) {
			mKillGroupMobSpawnData->mLastValidMobPosition = mKillGroupMobSpawnData->mMobTargetGroup[0]->GetActorLocation();
		}
		if (mKillGroupMobSpawnData->mMobTargetGroup.Num() == 0) {
			markCompleted();
		}
	}
}

void KillGroup::onStop() {
	const auto World = &game().world();

	if (const auto ItemDropActor = actorquery::getFirstActor<AItemDropActor>(World)) {
		for (auto player : World->GetGameInstance()->GetLocalPlayers()) {
			if(const auto item = GetItemRegistry().Request(stringutil::toFName((mData.rewardId)))) {
				ItemDropActor->DropItems(mKillGroupMobSpawnData->mLastValidMobPosition, item.GetValue(), mData.amount, player->GetPlayerController(World)->GetPawn(), true);
			}
		}
	}
	if (mDoorLocker) {
		mDoorLocker->unlock();
	}
}

TilePtr KillGroup::getTargetTile() const {
	std::vector<TilePtr> tiles = targetFinder().getTiles(targetlocators::allTilesInStretch(mData.stretch));
	if (tiles.empty()) {
		return nullptr;
	}
	std::random_shuffle(tiles.begin(), tiles.end());
	const auto it = std::find_if(tiles.begin(), tiles.end(), [](TilePtr tile) { return !tile->spawnRegions().isEmpty(); });
	return it != tiles.end() ? *it : tiles.front();
}

mobspawn::Regions KillGroup::getSpawnRegions() const {
	return mData.stretch.empty() ? mobspawn::Regions(targetFinder().get(mData.spawnRegions)) : mTargetTile->spawnRegions();
}

}}
