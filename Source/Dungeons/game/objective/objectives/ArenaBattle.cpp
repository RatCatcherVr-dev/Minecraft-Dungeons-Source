#include "Dungeons.h"
#include "ui/MissionProgressHandler.h"
#include "ui/MissionPresentationHandler.h"
#include "ArenaBattle.h"
#include "CommonTypes.h"
#include "game/Conversion.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/provider/Actors.h"
#include "game/Enchantments/mobs/MobEnchantmentTypes.h"
#include "game/mobspawn/MobAction.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "game/objective/ObjectiveDoorLocker.h"
#include "game/objective/ObjectiveUtil.h"
#include "game/objective/TargetLocators.h"
#include "game/util/LocationQuery.h"
#include "lovika/RegionPredicates.h"
#include "lovika/io/TileIo.h"
#include "util/Algo.h"
#include "util/CollectionUtils.h"
#include "world/entity/MobTags.h"
#include <functional>
#include "UnrealMathUtility.h"

namespace {
	bool areWaveSpawnRegionsValid(const std::vector<io::ObjectiveMobs>& waves, game::RegionFinder& finder) {
		for (auto&& wave : waves) {
			if (wave.spawnAtRegion) {
				if (!finder.single(wave.spawnAtRegion.GetValue())) {
					return false;
				}
			}
		}
		return true;
	}
}

namespace game { namespace objective {

using namespace std::placeholders;

ArenaBattle::ArenaBattle(const io::ObjectiveArenaBattleData& data)
	: mData(data)
	, mWaveIndex(0)
	, mLastWaveStartedTimestamp(0)
	, mLastWaveClearedTimeStamp(0)
	, mPreSpawnMobDuration(0)
	, bWaveInProgress(false)
	, mArenaTile(nullptr) 
{
	mArenaMobSpawnData = MakeShared<FSharedArenaMobSpawndata>();

	for (auto&& wave : mData.waves) {
		for (auto&& group : wave.groups) {
			mobspawn::prepareMobGroup(group);
		}
	}

}

Validation ArenaBattle::validate(ValidationType type) const {
	return validationErrorsIf({
		{ !mArenaTile, "Didn't find any (or too many) tiles for stretch: " + mData.stretch },
		{ mArenaTile && mSpawnRegions->isEmpty(), "Didn't find any arena spawn regions for stretch: " + mData.stretch },
		{ mArenaTile && mDoorLocker->isEmpty() && !mDoorLocker->isNull(), "Didn't find any gate regions (or the BP_Door_X didn't have any door components) in the tile" },
		{ mArenaTile && !mDoorLocker->couldPlaceAllDoors(), "Couldn't place all door objects" },
		{ mData.preSpawnMobObject && !mPreSpawnMobActorClass, "Incorrect file path when trying to load pre spawn mob" },
		{ !areWaveSpawnRegionsValid(mData.waves, targetFinder()), "Specific spawn regions for waves are invalid/can't be found"},
		{ algo::any_of(mData.mobActivations, RETLAMBDA(it == EntityType::Undefined)), "Couldn't parse all mob-activation entity types" }
	});
}

FObjectiveLocations ArenaBattle::getLocations() const {
	return TArray<FVector>{};
}

void ArenaBattle::onInit() {
	mArenaTile = findArenaTile();

	if (mArenaTile) {
		mSpawnRegions = make_unique<mobspawn::Regions>(mArenaTile->tilePlacement().filterRegions(regionpredicates::isArenaSpawn()));

		mDoorLocker = objectiveDoorLocker(
			game(),
			mData.gate,
			io::ObjectiveGate{ io::ObjectiveGate::Gates, {}, {}, true },
			mArenaTile);

		mSpawnConfig = mobspawn::configs::Default(game().settings().difficulty, true).Once().Actions({
				mobspawn::LongOffensiveRange(),
				mobspawn::RequiredForObjective(),
				mobspawn::BlockMusicOverride(mData.hasMusic)
		});

		if (eventType() == EEventType::Boss) {
			mSpawnConfig.actions.push_back(mobspawn::AddGlobalHealthBarOnMiniBosses());
		}

		if (mData.preSpawnMobObject) {
			FString filePath = mData.preSpawnMobObject.GetValue().c_str();
			mPreSpawnMobActorClass = ConstructorHelpersInternal::FindOrLoadClass(filePath, AActor::StaticClass());
			if (mPreSpawnMobActorClass) {
				mPreSpawnMobActorClass->AddToRoot();
			}
		}

		if (auto arenaNotifyLocation = Util::single(mArenaTile->tilePlacement().filterRegions(regionpredicates::name("arena-notify")))) {
			mArenaFocusLocation = centerFloor(arenaNotifyLocation->area());
		} else if (!mSpawnRegions->isEmpty()) {
			mArenaFocusLocation = centerFloor(mSpawnRegions->get(0).area());
		}

		findArenaPlacedMobs(); 
	}
}

void ArenaBattle::findArenaPlacedMobs() {
	const TilePtr tile = findArenaTile();
	TArray<EntityType> types(&mData.mobActivations.front(), mData.mobActivations.size());

	if (types.Num()) {
		const auto pred = [&tile, &types](AMobCharacter* v) {
			const BlockPos pos = conversion::ueToBlock(*v);
			return types.Contains(v->EntityType) && tile->bounds().containsXZ(pos);
		};

		const auto& baselist = InstanceTracker<AMobCharacter>::GetList(&game().world()).FilterByPredicate(pred);
		for (auto mob : baselist) {
			placedMobsInTile.Add(mob);
		}
	}
}

void ArenaBattle::onTick() {
	removeClearedMobs();

	if (mDelayedEndTimestamp) {
		if (currentTimeSeconds() >= mDelayedEndTimestamp.GetValue()) {
			UE_LOG(LogObjective, Display, TEXT("Finishing arena battle @ %f s (delayed end)!"), currentTimeSeconds());
			markCompleted();
		}
		return;
	}

	if (checkCompletion()) {
		if (mData.endDelaySeconds) {
			UE_LOG(LogObjective, Display, TEXT("Finishing arena battle in %f s (delayed end)!"), mData.endDelaySeconds.GetValue());
			mDelayedEndTimestamp = currentTimeSeconds() + mData.endDelaySeconds.GetValue();
		} else {
			UE_LOG(LogObjective, Display, TEXT("Finishing arena battle @ %f s!"), currentTimeSeconds());
			markCompleted();
		}
	}

	if (secondsSinceStart() >= (mData.startTime + mPreSpawnMobDuration) && shouldStartNextWave()) {
		startNextWave();
	}
	
	drainQueue(preSpawnQueue, std::bind(&ArenaBattle::preSpawn, this, _1));
	drainQueue(spawnQueue, std::bind(&ArenaBattle::spawn, this, _1));
}

float ArenaBattle::uiDelayTime() const {
	return mData.preSpawnMobDuration;
}

EEventType ArenaBattle::eventType() const {
	if (isChallenge())
		return EEventType::SideQuestBattle;

	return mData.isBoss ? EEventType::Boss : EEventType::ArenaBattle;
}

bool ArenaBattle::triggerMusicPlaybackEvents() const {
	return mData.hasMusic;
}

void ArenaBattle::onStart() {
	if (mDoorLocker) {
		mDoorLocker->lock();
	}

	for (auto mob : placedMobsInTile) {
		if (mob.IsValid()) {
			for (const auto& action : mSpawnConfig.actions) { action(*mob); }
			for (const auto& action : mSpawnConfig.postSpawnActions) { action(*mob); }
			mArenaMobSpawnData->mWaveMobs.Add(mob);
			mob->OnObjectiveActivated.Broadcast();
		}
	}

	mEffectsActor = AEffectsActor::GetInstance(&game().world());

	if (mPreSpawnMobActorClass) {
		mPreSpawnMobDuration = mData.preSpawnMobDuration;		
		game().world().GetTimerManager().SetTimer(mPreSpawnTimer, [this] { spawnPreBattleMob(); }, 2.0f + mData.startTime, false);
	}

	// trigger PresentationHandler oob quit music:
	mPresentationHandler->SetOOBMissionBoundsLimit(conversion::blockCuboidToUe(mArenaTile->bounds()));
}

void ArenaBattle::onStop() {
	if (mDoorLocker) {
		mDoorLocker->unlock();
	}
	game().world().GetTimerManager().ClearTimer(mPreSpawnTimer);
	if (progressHandler()) {
		if (auto endLocation = getObjectiveEventEndLocation()) {
			progressHandler()->SetObjectiveEventLocation(endLocation, info().panToDuration);
		}
	}
}

void ArenaBattle::removeClearedMobs() {
	//Cleanup mobs
	mArenaMobSpawnData->mWaveMobs.RemoveAllSwap(RETLAMBDA(!actorquery::is::alive(it.Get())));

	//Detect wave cleared
	//keeping the cleanup and detection separate also fixes the bug with having to wait for empty arena battles.
	if(bWaveInProgress && spawnQueue.empty() && isNoRemainingEnemyMobsInWave()){
		mLastWaveClearedTimeStamp = currentTimeSeconds();
		//since Num() in mWaveMobs is not enough to know if the wave is "cleared" due to box of hearts
		//we must track the wave state so that we do not keep setting the mLastWaveClearedTimeStamp.
		bWaveInProgress = false; 
	}	
}

bool ArenaBattle::isNoRemainingEnemyMobsInWave() const {
	const auto isMobRemaining = [&tiles = game().tiles()](const AMobCharacter& mob) {
		if (mob.GetMaster()) {
			return false;
		}
		const auto* tile = tiles.getTile(mob);
		return tile && tile->bounds().contains(mob);
	};
	return mArenaMobSpawnData->mRequestedMobSpawns == 0 && algo::none_of(mArenaMobSpawnData->mWaveMobs, RETLAMBDA(isMobRemaining(*it)));
}

bool ArenaBattle::checkCompletion() const {
	return (mWaveIndex >= mData.waves.size() && spawnQueue.empty() && secondsSince(mLastWaveClearedTimeStamp) >= 1 && isNoRemainingEnemyMobsInWave())
		|| (mData.battleMaxDuration > 0.0f ? secondsSinceStart() >= mData.battleMaxDuration : false);
}

bool ArenaBattle::_shouldStartNextWave_fromWaveStart() const {
	if (secondsSince(mLastWaveStartedTimestamp) >= mData.waveInterval) {
		return true;
	}
	return mLastWaveClearedTimeStamp >= mLastWaveStartedTimestamp && secondsSince(mLastWaveClearedTimeStamp) >= mData.restingTime;
}

bool ArenaBattle::_shouldStartNextWave_fromWaveCleared() const {
	return secondsSince(mLastWaveClearedTimeStamp) >= mData.waveInterval && isNoRemainingEnemyMobsInWave();
}

bool ArenaBattle::shouldStartNextWave() const {
	if (mWaveIndex == 0) {
		return true;
	} 
	return spawnQueue.empty() && 
		(mData.calculateTimeFromWaveStart ? _shouldStartNextWave_fromWaveStart() : _shouldStartNextWave_fromWaveCleared());
}

void ArenaBattle::startNextWave() {
	if (mWaveIndex >= mData.waves.size()) {
		return;
	}	
	if (mWaveIndex == 0 && progressHandler()) {
		if (auto startLocation = getObjectiveEventStartLocation()) {
			progressHandler()->SetObjectiveEventLocation(startLocation, info().panToDuration);
		}
	}

	auto& wave = mData.waves[mWaveIndex];
	std::function<Vec3(size_t)> locationProvider = RETLAMBDA(mSpawnRegions->getRandomPosFromRandomRegion());
	if (wave.spawnAtRegion) {
		// Region is validated earlier to always exist
		const auto region = targetFinder().single(wave.spawnAtRegion.GetValue());
		locationProvider = [centerVec = centerFloor(region->area())](size_t) {return centerVec; };
	}

	const auto locationCenterProvider = [=](size_t) -> Vec3 {
		const auto location = locationProvider(0U);		
		return { FMath::FloorToFloat(location.x) + .5f, location.y, FMath::FloorToFloat(location.z) + .5f };
	};

	float delay { 0.f };
	const auto& difficultyStats = game().settings().difficultyStats;
	for (auto group : mobspawn::calculateObjectiveWaveMobsWithEnchantments(wave.groups, wave.count, difficultyStats)) {

		if (hasMobTag(group.type, MobTags::HashTag_Ancient) && group.count > 1) {
			group.count = 1;
		}

		auto locations = algo::generate::generate_n(group.count, locationCenterProvider);
		delay += 0.1f;
		enqueueSpawn(delay, group, locations, wave.showSpawnIndicator);
	}

	mWaveIndex++;
	mLastWaveStartedTimestamp = currentTimeSeconds();
	bWaveInProgress = true;
}

void ArenaBattle::spawnPreBattleMob() {
	mPreSpawnMobActor = placeObject(game().world(), *mPreSpawnMobActorClass, mArenaFocusLocation, 0.0f);
}

void ArenaBattle::preSpawn(PreSpawnEntry entry) {
	if (mEffectsActor.IsValid()) {
		for (const auto& position : entry.blockPos) {
			const auto translation = conversion::posToUe(position);
			if (const auto ground = locationquery::findGround(game().world(), translation)) {
				mEffectsActor->PreSpawnEffectsAtLocation(
					{ mData.spawnEffectsPack.c_str() },
					entry.isLarge,
					{ translation.X, translation.Y, ground.GetValue() }
				);
			}
		}
	}
}

void ArenaBattle::spawn(SpawnEntry entry) {
	auto it = entry.blockPos.begin();
	const auto PositionProvider = [&] { return *it++; };

	AGameBP* gameBP = actorquery::getFirstActor<AGameBP>(&game().world());
	const bool bIsLarge = entry.isLarge;
	mArenaMobSpawnData->mRequestedMobSpawns += entry.group.count;

	TSharedPtr<FSharedArenaMobSpawndata> capturedSpawnData = mArenaMobSpawnData;
	std::string capturedSpawnEffectsPack = mData.spawnEffectsPack;
	TWeakObjectPtr<AEffectsActor> capturedEffectsActor = mEffectsActor;

	gameBP->RequestMobGroupSpawn(entry.group, mobspawn::providers::FromProviders(PositionProvider), mSpawnConfig, [](TArray<AMobCharacter*>& Mobs) {}, [capturedSpawnData, capturedSpawnEffectsPack, capturedEffectsActor, bIsLarge](AMobCharacter* pMob) {
	
		--capturedSpawnData->mRequestedMobSpawns;

		if (pMob)
		{
			capturedSpawnData->mWaveMobs.Add(pMob);

			if (capturedEffectsActor.IsValid())
			{
				capturedEffectsActor->SpawnEffectsAtLocation(
					{ capturedSpawnEffectsPack.c_str() },
					bIsLarge,
					pMob->GetActorLocation()
				);
			}
		}

	});
}

void ArenaBattle::enqueueSpawn(float delay, const mobspawn::SpawnGroup& group, const std::vector<Vec3>& blockPos, bool showSpawnIndicator) {
	const auto isLarge = hasMobTag(group.type, MobTags::HashTag_Miniboss);
	float extraDelay = 0;

	if (showSpawnIndicator) {
		preSpawnQueue.push({ delay, blockPos, isLarge });
		extraDelay = 1;
	}

	spawnQueue.push({ delay + extraDelay, group, blockPos, isLarge });
	UE_LOG(LogDungeons, Log, TEXT("ArenaBattle::enqueueSpawn: Post enqueue: %s"), *FString(EntityTypeToString(group.type).c_str()));
}

TilePtr ArenaBattle::findArenaTile() const {
	return targetFinder().singleTile(targetlocators::allTilesInStretch(mData.stretch));
}

FObjectiveEventLocation ArenaBattle::getObjectiveEventStartLocation() const {
	if (info().panToObjectiveStart) {
		return FObjectiveEventLocation(conversion::posToUe(mArenaFocusLocation), mData.isBoss ? EEventType::Boss : EEventType::ArenaBattle);
	}
	return {};
}

FObjectiveEventLocation ArenaBattle::getObjectiveEventEndLocation() const {
	if (info().panToObjectiveEnd && !mDoorLocker->isEmpty()) {
		const auto doorLocations = mDoorLocker->getDoorLocations();
		return doorLocations.Num() ? FObjectiveEventLocation(doorLocations[0], EEventType::DoorOpened) : FObjectiveEventLocation();
	}
	return {};
}

}}
