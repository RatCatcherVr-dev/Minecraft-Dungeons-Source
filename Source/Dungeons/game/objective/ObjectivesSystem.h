#pragma once

#include <Array.h>
#include "CommonTypes.h"
#include "ObjectiveCompletion.h"
#include "lovika/io/IoObjectiveTypes.h"

class AMissionProgressHandler;
class FOutputDevice;

struct ObjectiveInitInfo {
	bool passedValidation;
	io::ObjectiveInfo info;
};

namespace game {
class Game;

namespace objective {
class Objective;
enum class ValidationType: int;

class ObjectivesSystem {
public:
	ObjectivesSystem(Game&, TArray<Unique<Objective>>&&, FOutputDevice* = nullptr);
	~ObjectivesSystem();

	void tick();
	Completion completion() const;
	Objective* current() const;

	void forceCompleteAll() { mForceCompletion = true; }

	AMissionProgressHandler* GetMissionProgressHandler() const { return mProgressHandler; }

	TArray<ObjectiveInitInfo> start_DEBUG_DO_NOT_USE();
private:
	TArray<ObjectiveInitInfo> start();
	void next();

	Game& mGame;
	int mIndex;
	bool mIsFailed;
	FOutputDevice& mLog;
	TArray<Unique<Objective>> mObjectives;
	AMissionProgressHandler* mProgressHandler;

	bool mForceCompletion;
	bool mStarted;
	int64_t mTicksTilStart;
};

}}
