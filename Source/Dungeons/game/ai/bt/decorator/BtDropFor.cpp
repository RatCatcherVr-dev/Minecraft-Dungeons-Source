#include "Dungeons.h"
#include "BtDropFor.h"

UBtDropFor::UBtDropFor(bt::Duration delay)
	: delay(delay) {
	type = "drop-for";
}

bool UBtDropFor::ShouldRunChild(bt::StateRef state) {
	if (!initialised) {
		// add code path for delay == 0
		initialised = true;
		next = bt::TimeStamp::FromNow(state, delay);
		return false;
	}
	return next.IsPassed(state);	
}

void UBtDropFor::OnStop(bt::StateRef state) {
	Super::OnStop(state);
	initialised = false;
}