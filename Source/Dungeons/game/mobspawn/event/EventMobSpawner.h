#pragma once

#include "EventMobTrack.h"
#include "game/GameProgress.h"
#include "game/mobspawn/MobSpawnTypes.h"
#include "util/Random.h"
#include <vector>

class Random;

namespace game { class Game; }

namespace game { namespace mobspawn { namespace eventmob {

class EventMobSpawner {
public:
	EventMobSpawner(const Game&);

	Track& createTrack(const EventPredicateProvider& cooldown);
	void update();
private:
	EventStamp _getCurrentTime() const;
	EventState _getStateFor(Track&) const;

	TilePtr _findSpawnableTile(const EventState&) const;

	const Game& mGame;
	mutable Random mRandom;
	Config mSpawnConfig;
	EventStamp mGlobalLastSpawned;
	TArray<Unique<Track>> mTracks;
};

std::vector<EntityType> getAllHyperMissionEventMobCandidates();

Unique<EventMobSpawner> createDefaultEventMobSpawnerForLevel(const Game&);

}}}
