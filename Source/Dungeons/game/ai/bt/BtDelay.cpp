#include "Dungeons.h"
#include "BtDelay.h"

UDelay::UDelay( bt::Duration seconds )
	: duration( seconds )
	, endTime() {
}

bool UDelay::OnCanRun( bt::StateRef ) {
	return true;
}

bool UDelay::OnCanContinue( bt::StateRef state ) {
	return !endTime.IsPassed( state );
}

void UDelay::OnStart( bt::StateRef state ) {
	endTime = bt::TimeStamp::FromNow(state, duration);
}

void UDelay::OnStop( bt::StateRef ) {
}

Unique<UDelay> delay(bt::Duration seconds) {
	return make_unique<UDelay>( seconds );
}
