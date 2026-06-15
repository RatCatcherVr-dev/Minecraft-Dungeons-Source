#include "Dungeons.h"
#include "BtRotate.h"

UBtRotate::UBtRotate(ERotateOn rotateOn)
	: rotateOn(rotateOn) {
	type = "rotate";
}

void UBtRotate::OnStop(bt::StateRef state) {
	Super::Stop(state);

	if (rotateOn == ERotateOn::Start && ++index >= children.size()) {
		index = 0;
	}
}

void UBtRotate::OnTick(bt::StateRef state) {
	Super::_Tick(state);

	if (rotateOn == ERotateOn::Tick && ++index >= children.size()) {
		index = 0;
	}
}

UBtNode* UBtRotate::OnWillRunGetBranch(bt::StateRef state) {
	return RunnableOrNull(state, *children[index]);
}
