#include "Dungeons.h"
#include "ChallengeSystem.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/GameTypes.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "game/event/TileEvents.h"
#include "game/trigger/Triggers.h"
#include "game/util/ActorQuery.h"
#include "lovika/RegionPredicates.h"
#include "Objective.h"
#include "ObjectivesSystemUtil.h"
#include "ObjectiveValidationType.h"
#include "ObjectiveFactory.h"
#include "ObjectiveUtil.h"
#include "util/Algo.hpp"
#include "util/RandomUtil.h"
#include "ui/MissionPresentationHandler.h"

namespace game { namespace objective {

std::vector<TilePtr> getChallengeOrderedTiles(std::vector<TilePtr> tiles) {
	// We order the tiles here in some way to not give bias to the
	// earlier tiles (compared to the latter) in the original order.
	algo::random::shuffle(tiles);
	return tiles;
}

void validateAndStartChallengeIfNotStarted(Objective& challenge, const std::string& prefix, bool& started, FOutputDevice& log) {
	if (!started) {
		started = true;
		if (util::validate(prefix, challenge, ValidationType::Start, &log)) {
			challenge.start();
		}
	}
}

#if NO_LOGGING
#define LOG_CHECK( x ) 
#else
#define LOG_CHECK( x ) x
#endif

const std::vector<FString> ChallengeSystem::DEFAULT_REWARD_PREFABS = { "RewardChest/BP_FancyChest_Reward" , "RewardChest/BP_EmeraldChest_Reward" };
	
ChallengeSystem::ChallengeSystem(Game& game, FOutputDevice* log /*= nullptr*/)
	: mGame(game)
	, mLog(thisOrGlobalLog(log))
	, mTicksTilStart(5) {
}

UReplicatedInteractableComponent* findInteractable(UWorld& world, game::TileRef tile, FString path, FOutputDevice& log) {
	const auto cls = ConstructorHelpersInternal::FindOrLoadClass(path, AActor::StaticClass());
	if (!cls) {
		LOG_CHECK(log.CategorizedLogf(LogObjective.GetCategoryName(), ELogVerbosity::Error, TEXT("Couldn't (re)load trigger object class '%s' for tile id='%s'"), *path, UTF8_TO_TCHAR(tile.meta().id.c_str())); )
		return nullptr;
	}
	const auto obj = actorquery::getFirstActor(&world, cls, RETLAMBDA(tile.bounds().containsXZ(it)));
	if (!obj) {
		LOG_CHECK(log.CategorizedLogf(LogObjective.GetCategoryName(), ELogVerbosity::Error, TEXT("Couldn't find trigger object '%s' inside bounds for tile id='%s'"), *path, UTF8_TO_TCHAR(tile.meta().id.c_str())); )
		return nullptr;
	}
	const auto interactable = obj->FindComponentByClass<UReplicatedInteractableComponent>();
	if (!interactable) {
		LOG_CHECK(log.CategorizedLogf(LogObjective.GetCategoryName(), ELogVerbosity::Error, TEXT("Trigger object '%s' has no UInteractableComponent"), *path);)
		return nullptr;
	}
	return interactable;
}

void ChallengeSystem::start() {
	const auto& allTiles = mGame.tiles().getTiles();

	for (auto& tile : getChallengeOrderedTiles(mGame.tiles().getTiles())) {
		auto& meta = tile->meta();
		auto& challenges = tile->meta().metadata.challenges;
		if (challenges.empty()) {
			continue;
		}
		const auto prefix = "Challenge on tile <" + tile->tile().id() + '>';

		for (auto& challengeDef : challenges) {
			const auto considerTilesType = challengeDef.objective.info.considerTiles.Get(io::ObjectiveConsiderTilesType::Tile);
			const auto tilesToConsider = util::getTiles(allTiles, considerTilesType, tile);
			const RegionFinder regionFinder = regionfinders::Default(tilesToConsider);
			TUniquePtr<Item> item = MakeUnique<Item>(regionFinder, challengeDef.reward, create(challengeDef.objective));

			if (!item->objective) {
				LOG_CHECK( mLog.CategorizedLogf(LogObjective.GetCategoryName(), ELogVerbosity::Error, TEXT("Failed to factory create challenge objective for tile id='%s'"), UTF8_TO_TCHAR(meta.id.c_str())); )
				continue;
			}

			auto* presentationHandler = actorquery::getFirstActor<AGameBP>(&mGame.world())->GetPresentationHandler();
			item->objective->init(mGame, std::make_unique<RegionFinder>(regionfinders::Default(tilesToConsider)), -1, presentationHandler, nullptr);
			if (!util::validate(prefix, *item->objective, ValidationType::Init, &mLog)) {
				continue;
			}
			// Place trigger
			if (challengeDef.triggerObject) {
				auto path = game::PrefabPath(challengeDef.triggerObject.GetValue());

				if (!findInteractable(mGame.world(), *tile, path, mLog)) {
					continue;
				}

				mGame.tileEvents().tileEnterNew([this, tile, path, &ch = *item->objective, prefix](events::TileState ts) {
					if (&ts.tile != tile) {
						return;
					}
					if (const auto interactable = findInteractable(mGame.world(), *tile, path, mLog)) {
						interactable->OnInteractCallback = [&ch, prefix, started = false, this](const UReplicatedInteractableComponent& component, AActor* instigator) mutable {
							validateAndStartChallengeIfNotStarted(ch, prefix, started, mLog);
						};
					}
				});
			} else {
				const auto triggerRegion = regionFinder.single(challengeDef.trigger);
				if (!triggerRegion || !regionpredicates::isTrigger()(triggerRegion.GetValue())) {
					LOG_CHECK(mLog.CategorizedLogf(LogObjective.GetCategoryName(), ELogVerbosity::Error, TEXT("Couldn't find trigger region '%s' for tile id='%s'"), UTF8_TO_TCHAR(challengeDef.trigger.c_str()), UTF8_TO_TCHAR(meta.id.c_str()));)
					continue;
				}
				mGame.triggers().enterRegion({ triggerRegion.GetValue() }, [&ch = *item->objective, prefix, started = false, this](const trigger::RegionState& state) mutable {
					if (state.actor.IsA<APlayerCharacter>()) {
						validateAndStartChallengeIfNotStarted(ch, prefix, started, mLog);
					}
				});
			}
			item->lootRegion = item->targetFinder.single(item->reward.locator.Get("*.*.challengeloot"));

			mChallenges.Add(std::move(item));
		}
	}
}

void ChallengeSystem::tick() {
	mTicksTilStart--;
	if (mTicksTilStart == 0) {
		start();
	}
	if (mTicksTilStart > 0) {
		return;
	}

	for (auto& item : mChallenges) {
		item->objective->tick();

		if (item->objective->completion().isCompleted()) {
			item->objective->stop();
			spawnReward(*item);
		}
	}
	mChallenges.RemoveAllSwap(RETLAMBDA(it->objective->completion().isCompleted()));
}

Game& ChallengeSystem::game() const {
	return mGame;
}

TArray<FVector> ChallengeSystem::getRewardLocations() const {
	TArray<FVector> out;
	// @attn: Removed this on 5th April 2019 due to failing compilation after me
	//        removing implicit cast from Vec3 -> FVector and it doesn't seem we
	//        are gonna show challenges on the map (except perhaps chests after
	//        the challenge is completed)
	//for (auto& challenge : mChallenges) {
	//	if (challenge->lootRegion) {
	//		out.Add(center(challenge->lootRegion->area(), true).asFVector?());
	//	}
	//}
	return out;
}

void ChallengeSystem::spawnReward(Item& challenge) {
	static const auto getRelativePrefabPath = [](const io::ObjectiveReward& reward) -> FString {
		return reward.customPrefab.Get(*Util::randomChoice(DEFAULT_REWARD_PREFABS));
	};
	
	if (auto region = challenge.lootRegion) {
		const auto path = game::PrefabPath(getRelativePrefabPath(challenge.reward));
		placeObject(game().world(), path, centerFloor(region->area()), 180);
	}
}

//
// Internal "item", holding info about a challenge
//
ChallengeSystem::Item::Item(RegionFinder targetFinder, io::ObjectiveReward reward, Unique<Objective> objective)
	: targetFinder(std::move(targetFinder))
	, reward(std::move(reward))
	, objective(std::move(objective)) {
}

}}
