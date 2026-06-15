#include "Dungeons.h"
#include "BtBranch.h"

UBtBranch::UBtBranch() {
	type = "branch";
}

bool UBtBranch::OnWillRun(bt::StateRef state) {
	next = OnWillRunGetBranch(state);
	return next != nullptr;
}

void UBtBranch::OnTick(bt::StateRef state) {
	if (next != curr) {
		if (curr) {
			curr->Stop(state);
		}
		curr = next;
	}
	next->_Tick(state);
}

void UBtBranch::OnStop(bt::StateRef state) {
	Super::OnStop(state);
	curr = nullptr;
}

UBtNode* UBtBranch::RunnableOrNull(bt::StateRef state, UBtNode& node) {
	return node._WillRun(state) ? &node : nullptr;
}
