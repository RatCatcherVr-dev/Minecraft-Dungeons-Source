#include "Dungeons.h"
#include "Gauntlet.h"
#include "game/Conversion.h"
#include "game/Game.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/objective/ObjectiveDoorLocker.h"
#include "game/objective/ObjectiveUtil.h"

namespace game { namespace objective {

Gauntlet::Gauntlet(const io::ObjectiveGauntletData& data)
	: mData(data) {
}

Validation Gauntlet::validate(ValidationType) const {
	return validationErrorsIf({
		{ mEndRegions.empty(), "Can't find target regions at: " + mData.endRegion },
		{ mDoorLocker->isEmpty() && !mDoorLocker->isNull(), "Didn't find any gate regions (or the BP_Door_X didn't have any door components) in the tile" },
		{ !mDoorLocker->couldPlaceAllDoors(), "Couldn't place all door objects" },
	});
}

FObjectiveLocations Gauntlet::getLocations() const {
	return mMarkerLocations;
}

void Gauntlet::onInit() {
	mEndRegions = targetFinder().get(mData.endRegion);
	mMarkerLocations = toObjectiveLocations(targetFinder(), mData.markerRegion);

	mDoorLocker = objectiveDoorLocker(
		game(),
		mData.gate,
		io::ObjectiveGate{ io::ObjectiveGate::Nothing }
	);
}

void Gauntlet::onStart() {
	if (mDoorLocker) {
		mDoorLocker->lock();
	}
	mUnregisterKey = game().triggers().enterRegion(mEndRegions, [this](auto& state) {
		if (state.actor.template IsA<APlayerCharacter>()) {
			markCompleted();
		}
	});
}

void Gauntlet::onStop() {
	if (mDoorLocker) {
		mDoorLocker->unlock();
	}
	game().triggers().unregister(mUnregisterKey);
}

}}
