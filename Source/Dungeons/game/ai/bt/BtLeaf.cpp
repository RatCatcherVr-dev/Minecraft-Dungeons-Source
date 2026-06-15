#include "Dungeons.h"
#include "BtLeaf.h"

UBtLeaf::UBtLeaf() {
	type = "task";
}

void UBtLeaf::OnInitialise(bt::StateRef state) {
	if (shouldInit)
	{
		Init(state);
		shouldInit = false;
	}
}

bool UBtLeaf::OnWillRun(bt::StateRef state) {
	if (shouldInit) {
		return false; // dont run if we are not initialised
	}
	return IsRunning() ? OnCanContinue(state) : OnCanRun(state);
}

bool UBtLeaf::OnCanRun(bt::StateRef) {
	return true;
}

bool UBtLeaf::OnCanContinue(bt::StateRef state) {
	return OnCanRun(state);
}
