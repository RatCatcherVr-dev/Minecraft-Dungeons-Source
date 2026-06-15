#include "Dungeons.h"
#include "BtGreedySequence.h"

UBtGreedySequence::UBtGreedySequence() {
	type = "greedy-sequence";
}

bool UBtGreedySequence::OnWillRun(bt::StateRef state) {
	nextCount = 0;

	for (auto& child : children) {
		if (!child->_WillRun(state)) {
			break;
		}
		++nextCount;
	}
	return nextCount > 0;
}

void UBtGreedySequence::OnTick(bt::StateRef state) {
	if (nextCount < currCount) {
		for (int i = nextCount; i < currCount; ++i) {
			children[i]->Stop(state);
		}
	}
	currCount = nextCount;

	for (int i = 0; i < currCount; ++i) {
		children[i]->_Tick(state);
	}
}

void UBtGreedySequence::OnStop(bt::StateRef state) {
	Super::OnStop(state);
	currCount = 0;
}
