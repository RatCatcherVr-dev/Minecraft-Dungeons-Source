#include "Dungeons.h"
#include "BtStartIfElse.h"

UBtStartIfElse::UBtStartIfElse() {
	type = "start-if-else";
}

void UBtStartIfElse::OnStop(bt::StateRef state)
{
	Super::OnStop(state);
	cachedBranchIndex = -1;
}

UBtNode* UBtStartIfElse::OnWillRunGetBranch(bt::StateRef state) {
	if (cachedBranchIndex == -1)
	{
		cachedBranchIndex = children[0]->_WillRun(state) ? 1 : 2;
	}
	return RunnableOrNull(state, *children[cachedBranchIndex]);
}
