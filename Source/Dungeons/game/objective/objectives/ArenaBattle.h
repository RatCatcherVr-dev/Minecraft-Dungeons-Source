#pragma once

#include "game/objective/Objective.h"
#include "game/actor/EffectsActor.h"
#include "game/level/GameTile.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "lovika/io/IoObjectiveTypes.h"
#include "ui/MissionProgressHandler.h"
#include <WeakObjectPtrTemplates.h>
#include <vector>

class AMobCharacter;

namespace game { namespace objective {

class DoorLocker;

class ArenaBattle : public Objective {
public:
	ArenaBattle(const io::ObjectiveArenaBattleData&);

	virtual float uiDelayTime() const override;
	virtual EEventType eventType() const override;
	virtual bool triggerMusicPlaybackEvents() const override;
	Validation validate(ValidationType) const override;

	FObjectiveLocations getLocations() const override;
protected:
	void onInit() override;
	void onTick() override;
	void onStart() override;
	void onStop() override;
private:
	struct PreSpawnEntry {
		float delay;
		std::vector<Vec3> blockPos;
		bool isLarge;
	};

	struct SpawnEntry {
		float delay;
		mobspawn::SpawnGroup group;
		std::vector<Vec3> blockPos;
		bool isLarge;
	};

	void removeClearedMobs();
	bool isNoRemainingEnemyMobsInWave() const;
	bool checkCompletion() const;

	bool _shouldStartNextWave_fromWaveStart() const;
	bool _shouldStartNextWave_fromWaveCleared() const;
	bool shouldStartNextWave() const;
	void startNextWave();
	void findArenaPlacedMobs();

	template <typename Queue, typename Apply>
	void drainQueue(Queue& queue, Apply apply) {
		const auto now = game().world().GetTimeSeconds();

		while (!queue.empty()) {
			const auto entry = queue.front();

			if (entry.delay + mLastWaveStartedTimestamp > now) {
				break;
			}

			queue.pop();

			apply(entry);
		}
	}

	void spawnPreBattleMob();

	void preSpawn(PreSpawnEntry);
	void spawn(SpawnEntry);
	
	void enqueueSpawn(float delay, const mobspawn::SpawnGroup&, const std::vector<Vec3>& blockPos, bool showSpawnIndicator);

	TilePtr findArenaTile() const;

	FObjectiveEventLocation getObjectiveEventStartLocation() const;
	FObjectiveEventLocation getObjectiveEventEndLocation() const;

	io::ObjectiveArenaBattleData mData;
	int mWaveIndex;
	bool bWaveInProgress;
	float mLastWaveStartedTimestamp;
	float mLastWaveClearedTimeStamp;
	float mPreSpawnMobDuration;
	float mBattleMaxDuration;

	struct FSharedArenaMobSpawndata
	{
		int mRequestedMobSpawns = 0;
		TArray<TWeakObjectPtr<AMobCharacter>> mWaveMobs;
	};

	TSharedPtr<FSharedArenaMobSpawndata> mArenaMobSpawnData;

	TilePtr mArenaTile;
	Unique<DoorLocker> mDoorLocker;
	Unique<mobspawn::Regions> mSpawnRegions;
	mobspawn::Config mSpawnConfig;
	FTimerHandle mPreSpawnTimer;

	std::queue<PreSpawnEntry> preSpawnQueue;
	std::queue<SpawnEntry> spawnQueue;

	TWeakObjectPtr<AEffectsActor> mEffectsActor;

	TArray<TWeakObjectPtr<AMobCharacter>> placedMobsInTile;

	// Need to cache this position early since it's used when panning the camera
	Vec3 mArenaFocusLocation;
	UClass* mPreSpawnMobActorClass = nullptr;
	TWeakObjectPtr<AActor> mPreSpawnMobActor;

	TOptional<float> mDelayedEndTimestamp;
};

}}
