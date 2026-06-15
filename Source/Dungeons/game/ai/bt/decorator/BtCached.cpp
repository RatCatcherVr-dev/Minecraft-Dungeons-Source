#include "Dungeons.h"
#include "BtCached.h"

bool UBtCached::OnWillRun(bt::StateRef state) {
	shouldRunChild = ShouldRunChild(state);

	if (shouldRunChild) {
		lastWillRun = children[0]->_WillRun(state);
	}
	return lastWillRun;
}

void UBtCached::OnTick(bt::StateRef state) {
	if (shouldRunChild) {
		children[0]->_Tick(state);
	}
}
