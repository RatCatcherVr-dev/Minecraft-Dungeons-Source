#include "Dungeons.h"
#include "BtParallel.h"

UBtParallel::UBtParallel() {
	type = "parallel";
}

bool UBtParallel::OnWillRun(bt::StateRef state) {
	bool any = false;
	for (auto&& child : children) {
		if (child->_WillRun(state)) {
			any = true;
		}
	}
	return any;
}

void UBtParallel::OnTick(bt::StateRef state) {
	for (auto& child : children) {
		child->_Tick(state);
	}
}
