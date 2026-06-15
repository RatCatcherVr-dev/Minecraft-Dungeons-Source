#include "Dungeons.h"
#include "BtEvery.h"

UBtEvery::UBtEvery(bt::Duration interval)
	: interval(interval) {
	type = "every";
}

bool UBtEvery::ShouldRunChild(bt::StateRef state) {
	if (next.IsPassed(state)) {
		next = bt::TimeStamp::FromNow(state, interval);
		return true;
	}
	return false;
}
