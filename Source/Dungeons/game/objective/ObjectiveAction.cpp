#include "Dungeons.h"
#include "ObjectiveAction.h"
#include "game/Game.h"

namespace game { namespace objective {

void ObjectiveAction::init(Game& game) {
	mGame = &game;
	onInit();
}

RegionFinder& ObjectiveAction::targetFinder() const {
	if (!mTargetFinder) {
		//@attn: this is probably "wrong", since objective doesn't usually control what tiles it can access / aron@20181209
		mTargetFinder = make_unique<RegionFinder>(regionfinders::Default(game().tiles()));
	}
	return *mTargetFinder;
}

}}
