#include "EventTypes.h"

FObjectiveEventLocation::FObjectiveEventLocation() 
	: Location(FVector::ZeroVector)
	, EventType(EEventType::None)
	, bIsSet(false) {
}

FObjectiveEventLocation::FObjectiveEventLocation(FVector location, EEventType type) 
	: Location(location)
	, EventType(type)
	, bIsSet(true) {
}

bool FObjectiveEventLocation::IsSet() const {
	return bIsSet;
}
