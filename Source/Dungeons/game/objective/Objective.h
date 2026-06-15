#pragma once

#include "RegionFinder.h"
#include "ObjectiveCompletion.h"
#include "ObjectiveLocations.h"
#include "ObjectiveValidationType.h"
#include "lovika/io/IoObjectiveTypes.h"
#include "lovika/tile/TileDef.h"
#include "util/Validation.h"

DECLARE_LOG_CATEGORY_EXTERN(LogObjective, Log, All);

class UDoorComponent;
class AActor;
class AMissionProgressHandler;
class UMissionPresentationHandler;
enum class EEventType : uint8;

namespace game {

class Game;

namespace objective {

class ObjectiveAction;

class Objective {
public:
	Objective();
	virtual ~Objective();

	void setInfo(const io::ObjectiveInfo&);
	const io::ObjectiveInfo& info() const;

	virtual float uiDelayTime() const { return 0.0f; }
	virtual EEventType eventType() const;
	virtual Validation validate(ValidationType) const = 0;

	void init(Game&, Unique<RegionFinder>, int forcedEndCompletionTileIndex, UMissionPresentationHandler*, AMissionProgressHandler*);
	void start();
	void tick();
	void stop();

	void forceCompleted() { mIsForceCompleted = true; }

	Completion completion() const;

	virtual FObjectiveLocations getLocations() const { return {}; }
	virtual bool triggerMusicPlaybackEvents() const;

	Unique<class SoundData> narrator;
	Unique<class SoundData> objectiveTrack;
	Unique<class SoundData> objectiveCompletedTrack;

	RegionFinder& targetFinder() const;

	void addAction(Unique<ObjectiveAction>);
protected:
	virtual void onInit() {}
	virtual void onStart() {}
	virtual void onTick() {}
	virtual void onStop() {}
	
	float currentTimeSeconds() const;
	float secondsSinceStart() const;
	float secondsSince(float time) const;

	bool isStarted() const;
	bool isChallenge() const { return mProgressHandler == nullptr; } // better determination?
	//// need to handle leaving unfinished 'challenge
	UMissionPresentationHandler* mPresentationHandler = nullptr;
	
	Game& game() const { return *mGame; }
	AMissionProgressHandler* progressHandler() const { return mProgressHandler; }
	UMissionPresentationHandler* PresentationHandler() const { return mPresentationHandler; }

	void markFailed();
	void markCompleted();
	void markPartiallyCompleted(int count, int max);

	void _updateProgressHandler();
	//virtual bool AssetPathIsValid(const FString& assetPath) const;
private:
	io::ObjectiveInfo mInfo;
	Game* mGame = nullptr;
	Unique<RegionFinder> mTargetFinder;

	int mForcedCompletionTileIndex = -1;
	bool mIsForceCompleted = false;
	bool mIsFailed = false;
	bool mIsStarted = false;
	Completion mCompletion;

	float mStartTimeTimestamp = -1;
	//bool mForceCompleted = false;
	std::vector<Unique<ObjectiveAction>> mActions;

	AMissionProgressHandler* mProgressHandler = nullptr;
};

}}
