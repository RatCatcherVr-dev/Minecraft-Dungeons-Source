#pragma once

#include "RegionFinder.h"
#include "CommonTypes.h"

namespace game {

class Game;

namespace objective {

class ObjectiveAction {
public:
	virtual ~ObjectiveAction() {}

	void init(Game&);

	virtual void onStart() {}
	virtual void onTick() {}
	virtual void onStop() {}

protected:
	virtual void onInit() {}

	Game& game() const { return *mGame; }
	RegionFinder& targetFinder() const;
private:
	Game* mGame = nullptr;
	mutable Unique<RegionFinder> mTargetFinder;
};

}}
