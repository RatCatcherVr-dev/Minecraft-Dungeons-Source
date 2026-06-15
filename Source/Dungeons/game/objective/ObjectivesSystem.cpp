#include "Dungeons.h"
#include "ObjectivesSystem.h"
#include "Objective.h"
#include "ObjectivesSystemUtil.h"
#include "game/Game.h"
#include "game/GameBP.h"
#include "game/objective/objectives/SoundData.h"
#include "game/util/ActorQuery.h"
#include "ui/MissionProgressHandler.h"
#include "ui/MissionPresentationHandler.h"
#include "util/CollectionUtils.h"

namespace game { namespace objective {

ObjectivesSystem::ObjectivesSystem(Game& game, TArray<Unique<Objective>>&& objectives, FOutputDevice* log /*= nullptr*/)
	: mGame(game)
	, mLog(thisOrGlobalLog(log))
	, mIndex(-1)
	, mIsFailed(false)
	, mObjectives(std::move(objectives))
	, mProgressHandler(actorquery::getFirstActor<AMissionProgressHandler>(&game.world()))
	, mForceCompletion(false) 
	, mTicksTilStart(5) {
}

ObjectivesSystem::~ObjectivesSystem() {
}

TArray<ObjectiveInitInfo> ObjectivesSystem::start() {
	const auto& allTiles = mGame.tiles().getTiles();

	for (auto&& objective : mObjectives) {
		const auto defaultConsiderTilesType = mGame.missionDef().isHyperMission() ? io::ObjectiveConsiderTilesType::All : io::ObjectiveConsiderTilesType::Main;
		auto considerationTiles = util::getTiles(allTiles, objective->info().considerTiles.Get(defaultConsiderTilesType));
		auto* presentationHandler = actorquery::getFirstActor<AGameBP>(&mGame.world())->GetPresentationHandler();
		objective->init(mGame, make_unique<RegionFinder>(regionfinders::Default(std::move(considerationTiles))), -1, presentationHandler, mProgressHandler);
	}

	const auto initInfos = algo::map_tarray(mObjectives, RETLAMBDA((ObjectiveInitInfo{
		util::validate("Objective", *it, ValidationType::Init, &mLog),
		it->info()
	})));

	for (int i = mObjectives.Num() - 1; i >= 0; --i) {
		if (!initInfos[i].passedValidation) {
			mObjectives.RemoveAt(i);
		}
	}
	next();

	return initInfos;
}

void ObjectivesSystem::tick() {
	mTicksTilStart--;

	if (mTicksTilStart == 0) {
		start();
	}
	if (mTicksTilStart > 0) {
		return;
	}
	auto objective = current();
	if (!objective) {
		return;
	}
	if (mForceCompletion) {
		objective->forceCompleted();
	}

	objective->tick();

	if (objective->completion().isFailed()) {
		mIsFailed = true;
	} else {
		if (mProgressHandler) {
			mProgressHandler->SetPartiallyComplete(objective->completion().current(), objective->info().objectiveIndex);
		}
		if (objective->completion().isCompleted()) {
			next();
		}
	}
}

Completion ObjectivesSystem::completion() const {
	return (mIsFailed || mObjectives.Num() == 0) ? Completion::Failed() : Completion(mIndex, mObjectives.Num());
}

Objective* ObjectivesSystem::current() const {
	return !mIsFailed && mObjectives.IsValidIndex(mIndex)? mObjectives[mIndex].get() : nullptr;
}

TArray<ObjectiveInitInfo> ObjectivesSystem::start_DEBUG_DO_NOT_USE() {
	return start();
}

void ObjectivesSystem::next() {
	if (!completion().isPending()) {
		return;
	}
	if (auto objective = current()) {
		objective->stop();
	}
	++mIndex;

	if (auto objective = current()) {

#if !NO_LOGGING
		mLog.CategorizedLogf(LogObjective.GetCategoryName(), ELogVerbosity::Display, TEXT("Starting next objective: %s"), *objective->info().name.ToString());
#endif
		if (util::validate("Objective", *objective, ValidationType::Start, &mLog)) {
			objective->start();
		} else {
			next();
		}
	} else if (mObjectives.Num() > 0) {
		mProgressHandler->SetEmptyObjective();
		mProgressHandler->MissionFinished();

#if !NO_LOGGING
		mLog.CategorizedLogf(LogObjective.GetCategoryName(), ELogVerbosity::Display, TEXT("All (working) objectives completed!"));
#endif
	}
}

}}
