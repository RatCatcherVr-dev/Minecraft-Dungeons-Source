#include "Dungeons.h"
#include "BtIfElse.h"

UBtIfElse::UBtIfElse() {
	type = "if-else";
}

UBtNode* UBtIfElse::OnWillRunGetBranch(bt::StateRef state) {
	int branchIndex = children[0]->_WillRun(state) ? 1 : 2;
	return RunnableOrNull(state, *children[branchIndex]);
}
