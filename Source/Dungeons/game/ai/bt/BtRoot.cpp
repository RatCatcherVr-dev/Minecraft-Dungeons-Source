#include "Dungeons.h"
#include "BtRoot.h"

UBtRoot::UBtRoot() {
	type = "latent-root";
}

void UBtRoot::SetLatentTask(bt::StateRef state, UBtNode* node) {
	if (node == latentTask) {
		return;
	}
	StopLatentTask(state);

	latentTask = node;
	if (node) {
		node->isCurrentlyLatent = true;
	}
}

UBtNode* UBtRoot::OnWillRunGetBranch(bt::StateRef state) {
	if (latentTask && latentTask->_WillRun(state)) {
		return latentTask;
	}
	StopLatentTask(state);
	return RunnableOrNull(state, *children[0]);
}

void UBtRoot::FillData(FBtEvalTraversalState& state) {
	state.currentRoot = this;
}

void UBtRoot::StopLatentTask(bt::StateRef state) {
	if (!latentTask) {
		return;
	}
	latentTask->isCurrentlyLatent = false;
	latentTask->Stop(state);
	latentTask = nullptr;
}


void UBtRoot::OnStop(bt::StateRef state) {	
	StopLatentTask(state);
	Super::OnStop(state);
}
