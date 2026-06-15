#include "Dungeons.h"
#include "EventMobSpawner.h"
#include "EventMobType.h"
#include "EventMobProviders.h"
#include "game/Game.h"
#include "game/GameProgress.h"
#include "game/GameTypes.h"
#include "game/mission/MissionDefs.h"
#include "game/mission/hyper/HyperMissionUtil.h"
#include "game/mobspawn/MobGroupUtil.h"
#include "game/mobspawn/MobSpawner.h"
#include "game/mobspawn/MobSpawnConfigs.h"
#include "game/mobspawn/MobSpawnProviders.h"
#include "util/RandomUtil.h"
#include "world/entity/MobTags.h"

namespace game { namespace mobspawn { namespace eventmob {

EventMobSpawner::EventMobSpawner(const Game& game)
	: mGame(game)
	, mRandom(101 * game.settings().randomSeed)
	, mSpawnConfig(configs::DefaultTileData(game))
	, mGlobalLastSpawned(_getCurrentTime()) {
}

Track& EventMobSpawner::createTrack(const EventPredicateProvider& cooldown) {
	mTracks.Add(Unique<Track>(new Track(cooldown, _getCurrentTime())));
	return *mTracks.Last();
}

void EventMobSpawner::update() {
	for (const auto& track : mTracks) {
		const auto state = _getStateFor(*track);
		auto furthestTileIndex = state.current.tiles;

		if (!track->canRun(state)) {
			continue;
		}
		auto type = track->randomChoice(state);
		if (!type) {
			continue;
		}
		int spawnedCount = 0;
		for (const auto entityType : calculateMobs(type->groups, 1, mGame.settings().difficulty, &state.rnd)) {
			if (TilePtr tile = _findSpawnableTile(state)) {
				const auto transformProvider = mobspawn::providers::FromProviders(
					mobspawn::providers::position::Regions(tile->spawnRegions())
				);
				if (spawnNow(mGame.world(), entityType, transformProvider, mSpawnConfig)) {
					spawnedCount++;
					furthestTileIndex = Math::max(furthestTileIndex, tile->progress().global().index());
				}
			}
		}
		if (spawnedCount) {
			EventState newState = state;
			newState.current.tiles = furthestTileIndex;
			mGlobalLastSpawned = newState.current;
			track->trigger(newState);
			type->trigger(newState);
		}
	}
}

EventStamp EventMobSpawner::_getCurrentTime() const {
	return EventStamp{ mGame.progress().furthest().progress().global().index(), mGame.world().TimeSeconds };
}

EventStamp _getTimeSince(EventStamp current, EventStamp last) {
	return EventStamp{ std::max(0, current.tiles - last.tiles), current.seconds - last.seconds };
}

EventState EventMobSpawner::_getStateFor(Track& track) const {
	const auto current = _getCurrentTime();
	return EventState {
		mRandom,
		mGame.progress().furthest(),
		current,
		_getTimeSince(current, mGlobalLastSpawned),
		_getTimeSince(current, track.mLastSpawned)
	};
}

std::vector<TilePtr> getNextTiles(const Tiles& tiles, TileRef start, int maxCount) {
	std::vector<TilePtr> out;

	auto tile = &start;
	for (int i = 0; i < maxCount; ++i) {
		auto next = tiles.getNextDoor(*tile);
		if (!next) {
			break;
		}
		tile = &next->tile;
		out.push_back(tile);
	}
	return out;
}

TilePtr EventMobSpawner::_findSpawnableTile(const EventState& state) const {
	for (auto tile : getNextTiles(mGame.tiles(), state.furthest, 3)) {
		if (tile->isObjectiveTileHACK()) {
			continue;
		}
		if (tile->spawnRegions()) {
			return tile;
		}
	}
	return {};
}

const auto DefaultEventMobPredicate = providers::TilesSince(4, 14);

void addBonusMobs(EventMobSpawner& spawner) {
	spawner.
		createTrack(providers::TilesSince(9, 11)).
			addMobs({{{{ EntityType::PiggyBank }}}});
}

void addDefaultMobs(EventMobSpawner& spawner, const Game& game) {
	if (const auto mobGroup = game.missionDef().getEventMobGroup()) {
		spawner.
			createTrack(DefaultEventMobPredicate).
				addMobs({ mobGroup.GetValue() });
	}
}

std::vector<EntityType> getAllHyperMissionEventMobCandidates() {
	return algo::copy_if(TypeMap::singleton().mobTypes(),
		RETLAMBDA(hasMobTag(it, MobTags::HashTag_EventMob) && !hasMobTag(it, MobTags::HashTag_Underwater) && !hasMobTag(it, MobTags::HashTag_NoHyperMission)));
}

void addHyperMissionEventMobs(EventMobSpawner& spawner, const Game& game) {
	const auto eventMobTypes = algo::map_vector(getAllHyperMissionEventMobCandidates(), RETLAMBDA(io::MobType{ it }));

	auto createEventMobGroupOfSize = [eventMobTypes, rnd = Random(33 * game.settings().randomSeed)](int size) mutable {
		return io::MobGroup(util::randomSample(eventMobTypes, size, rnd));
	};

	for (const auto& levelId : util::getUniqueSubMissionIds(game.tiles())) {
		const auto onSubLevelButNotOnTheFirstFewTilesInThatSubLevel = providers::And(
			providers::OnSubLevel(levelId),
			providers::SkipTiles(3));

		spawner.
			createTrack(providers::And(DefaultEventMobPredicate, onSubLevelButNotOnTheFirstFewTilesInThatSubLevel)).
				addMobs({ createEventMobGroupOfSize(2) });
	}
}


//
// Factory
//
Unique<EventMobSpawner> createDefaultEventMobSpawnerForLevel(const Game& game) {
	if (game.missionDef().isTutorial()) {
		return std::make_unique<EventMobSpawner>(game);
	}
	auto spawner = std::make_unique<EventMobSpawner>(game);

	addBonusMobs(*spawner);

	if (game.missionDef().isHyperMission()) {
		addHyperMissionEventMobs(*spawner, game);
	} else {
		addDefaultMobs(*spawner, game);
	}

	return spawner;
}

}}}
