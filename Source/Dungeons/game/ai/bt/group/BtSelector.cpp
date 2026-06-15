#include "Dungeons.h"
#include "BtSelector.h"

UBtSelector::UBtSelector() {
	type = "selector";
}

UBtNode* UBtSelector::OnWillRunGetBranch(bt::StateRef state) {
	for (auto& child : children) {
		if (child->_WillRun(state)) {
			return child.get();
		}
	}
	return nullptr;
}
