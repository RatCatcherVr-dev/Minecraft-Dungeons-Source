#include "Dungeons.h"
#include "BtUninterruptible.h"
#include "BtRoot.h"

UBtUninterruptible::UBtUninterruptible() {
	type = "uninterruptible";
}

bool UBtUninterruptible::OnWillRun(bt::StateRef state) {
	return children[0]->_WillRun(state);
}

void UBtUninterruptible::OnStart(bt::StateRef state) {
	checkf(state.traversalState.currentRoot, TEXT("Trying to start a latent task without having a root!"));
	state.traversalState.currentRoot->SetLatentTask(state, children[0].get());
}

void UBtUninterruptible::OnTick(bt::StateRef state) {
	return children[0]->_Tick(state);
}
