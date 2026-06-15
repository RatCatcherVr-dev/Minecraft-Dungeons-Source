#include "Dungeons.h"
#include "Objective.h"
#include "ObjectiveAction.h"
#include "ObjectiveUtil.h"
#include "game/objective/objectives/SoundData.h"
#include "game/objective/EventTypes.h"
#include "game/Game.h"
#include "game/GameProgress.h"
#include "ui/MissionProgressHandler.h"
#include "ui/MissionPresentationHandler.h"
#include <LogMacros.h>

DEFINE_LOG_CATEGORY(LogObjective)

TAutoConsoleVariable<int32> CVarObjectivesLogStartStop(
	TEXT("Dungeons.Objectives.LogStartStop"),
	0,
	TEXT("Sets the start and stop log messages to enabled or disabled\n")
	TEXT("0: disabled\n")
	TEXT("1: enabled\n"),
	ECVF_Cheat
);

namespace game { namespace objective {

Objective::Objective()
	: mCompletion(0, 1) {
}

Objective::~Objective() {
}

void Objective::setInfo(const io::ObjectiveInfo& info) {
	mInfo = info;
}

const io::ObjectiveInfo& Objective::info() const {
	return mInfo;
}

EEventType Objective::eventType() const { 
	return EEventType::None; 
}


void Objective::init(Game& game, Unique<RegionFinder> targetFinder, int forcedEndCompletionTileIndex, UMissionPresentationHandler* presentationHandler, AMissionProgressHandler* progressHandler) {
	mGame = &game;
	mTargetFinder = std::move(targetFinder);
	mForcedCompletionTileIndex = forcedEndCompletionTileIndex;
	mPresentationHandler = presentationHandler;
	mProgressHandler = progressHandler;

	onInit();

	for (auto& action : mActions) {
		action->init(game);
	}
	if (narrator) {
		narrator->onInit();
	}
	if (objectiveTrack) {
		objectiveTrack->onInit();
	}
	if (objectiveCompletedTrack) {
		objectiveCompletedTrack->onInit();
	}
}

void Objective::start() {
	if (isStarted()) {
		return;
	}
	mIsStarted = true;
	mStartTimeTimestamp = currentTimeSeconds();

	if (CVarObjectivesLogStartStop.GetValueOnGameThread()) {
		UE_LOG(LogObjective, Display, TEXT("Objective/challenge started: %s"), *identifiableName(info()));
	}

	_updateProgressHandler();
	PresentationHandler()->ObjectiveStarted(*this);
	onStart();

	for (auto& action : mActions) {
		action->onStart();
	}
}

void Objective::tick() {
	if (!isStarted()) {
		return;
	}
	if (completion().isCompleted()) {
		return;
	}
	if (mForcedCompletionTileIndex >= 0 && game().progress().furthest().progress().global().index() >= mForcedCompletionTileIndex) {
		mIsForceCompleted = true;
	} else {
		onTick();

		for (auto& action : mActions) {
			action->onTick();
		}
	}
}

void Objective::stop() {
	if (CVarObjectivesLogStartStop.GetValueOnGameThread()) {
		UE_LOG(LogObjective, Display, TEXT("Objective/challenge stopped: %s"), *identifiableName(info()));
	}
	onStop();

	for (auto& action : mActions) {
		action->onStop();
	}

	PresentationHandler()->ObjectiveFinished(*this);
}

RegionFinder& Objective::targetFinder() const {
	return *mTargetFinder;
}

//
// Time
//
float Objective::currentTimeSeconds() const {
	return game().world().GetTimeSeconds();
}

float Objective::secondsSinceStart() const {
	return secondsSince(mStartTimeTimestamp);
}

float Objective::secondsSince(float time) const {
	return currentTimeSeconds() - time;
}

bool Objective::isStarted() const {
	return mIsStarted;
}

//
// Completion
//
Completion Objective::completion() const {
	return mIsForceCompleted ? Completion(mCompletion.max(), mCompletion.max()) : mCompletion;
}

void Objective::markFailed() {
	mCompletion = Completion::Failed();
}

void Objective::markCompleted() {
	mCompletion = Completion(mCompletion.max(), mCompletion.max());
}

void Objective::markPartiallyCompleted(int count, int max) {
	if (!mCompletion.isCompleted()) {
		mCompletion = Completion(count, max);
	}
}

void Objective::addAction(Unique<ObjectiveAction> action) {
	mActions.push_back(std::move(action));
}

bool Objective::triggerMusicPlaybackEvents() const {
	return false;
}

//
// Progress handler / UI
//
void Objective::_updateProgressHandler() {
	if (!mProgressHandler) {
		return;
	}
	if (!info().isHidden) {
		mProgressHandler->SetObjective(*this);
		mProgressHandler->SetPartiallyComplete(completion().current(), mInfo.objectiveIndex);
	} else {
		mProgressHandler->SetEmptyObjective();
	}
}

}}
