#include "Dungeons.h"
#include "BtCondition.h"

UBtCondition::UBtCondition() {
	type = "condition";
}

bool UBtCondition::OnWillRun(bt::StateRef state) {
	if (IsRunning() && children.size() > 1) { // IsRunning() -> "canContinueToRun"
		return children[1]->_WillRun(state);
	}
	return children[0]->_WillRun(state);
}
