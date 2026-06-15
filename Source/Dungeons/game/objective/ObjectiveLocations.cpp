#include "Dungeons.h"
#include "ObjectiveLocations.h"

FObjectiveLocations::FObjectiveLocations()
	: bUseLocations(false) {
}

FObjectiveLocations::FObjectiveLocations(TArray<FVector> locations)
	: Locations(std::move(locations))
	, bUseLocations(true) {
}

bool FObjectiveLocations::operator==(const FObjectiveLocations& rhs) const {
	if (bUseLocations) {
		return rhs.bUseLocations && Locations == rhs.Locations;
	}
	return !rhs.bUseLocations;
}

bool FObjectiveLocations::operator!=(const FObjectiveLocations& rhs) const {
	return !(*this == rhs);
}
