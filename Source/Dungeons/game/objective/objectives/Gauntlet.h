#pragma once

#include "game/objective/Objective.h"
#include "game/trigger/Triggers.h"
#include "lovika/io/IoObjectiveTypes.h"

namespace game { namespace objective {

class DoorLocker;

class Gauntlet : public Objective {
public:
	Gauntlet(const io::ObjectiveGauntletData&);

	Validation validate(ValidationType) const override;

	FObjectiveLocations getLocations() const;
protected:
	void onInit() override;
	void onStart() override;
	void onStop() override;
private:
	trigger::Triggers::UnregisterKey mUnregisterKey;
	io::ObjectiveGauntletData mData;
	std::vector<lovika::Region> mEndRegions;
	FObjectiveLocations mMarkerLocations;
	Unique<DoorLocker> mDoorLocker;
};

}}
