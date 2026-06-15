#include "Dungeons.h"
#include "TimeLimited.h"

UTimeLimited::UTimeLimited(bt::Duration duration, ETimeLimit betweenWhat)
	: interval(duration)
	, betweenWhat(betweenWhat) {
	type = "time-limited";
}

void UTimeLimited::OnStart(bt::StateRef state) {
	if (betweenWhat != ETimeLimit::MinTimeBetweenStopAndStart) {
		nextTimeLimit = bt::TimeStamp::FromNow(state, interval);
	}
}

void UTimeLimited::OnStop(bt::StateRef state) {
	if (betweenWhat == ETimeLimit::MinTimeBetweenStopAndStart) {
		nextTimeLimit = bt::TimeStamp::FromNow(state, interval);
	}
}

bool UTimeLimited::OnCanRun(bt::StateRef state) {
	return (betweenWhat == ETimeLimit::MaxTimeBetweenStartAndStop) ? true : nextTimeLimit.IsPassed(state);
}

bool UTimeLimited::OnCanContinue(bt::StateRef state) {
	return (betweenWhat == ETimeLimit::MaxTimeBetweenStartAndStop) ? !nextTimeLimit.IsPassed(state) : true;
}

//
// Factory
//
Unique<UTimeLimited> minTimeBetweenStopAndStart(bt::Duration duration) {
	return make_unique<UTimeLimited>(duration, ETimeLimit::MinTimeBetweenStopAndStart);
}

Unique<UTimeLimited> minTimeBetweenStarts(bt::Duration duration) {
	return make_unique<UTimeLimited>(duration, ETimeLimit::MinTimeBetweenStarts);
}

Unique<UTimeLimited> maxRunTime(bt::Duration duration) {
	return make_unique<UTimeLimited>(duration, ETimeLimit::MaxTimeBetweenStartAndStop);
}
