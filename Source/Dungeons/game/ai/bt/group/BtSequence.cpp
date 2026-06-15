#include "Dungeons.h"
#include "BtSequence.h"

UBtSequence::UBtSequence() {
	type = "sequence";
}

bool UBtSequence::OnWillRun(bt::StateRef state) {
	for (auto& child : children) {
		if (!child->_WillRun(state)) {
			return false;
		}
	}
	return true;
}

void UBtSequence::OnTick(bt::StateRef state) {
	for (auto& child : children) {
		child->_Tick(state);
	}
}
