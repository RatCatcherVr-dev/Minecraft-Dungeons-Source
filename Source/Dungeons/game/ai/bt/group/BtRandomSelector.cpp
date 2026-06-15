#include "Dungeons.h"
#include "BtRandomSelector.h"
#include "util/SharedRandom.h"

UBtRandomSelector::UBtRandomSelector() 
	: currentChild(0) {
	type = "random-selector";
}

UBtNode* UBtRandomSelector::OnWillRunGetBranch(bt::StateRef state) {
	if (children.empty()) {
		return nullptr;
	}

	if (IsRunning()) {
		if (currentChild >= 0 && currentChild < children.size()) {
			if (children[currentChild]->_WillRun(state)) {
				return children[currentChild].get();
			}
		}
	}
	else {
		std::vector<int> activeChildIndices;
		int index = 0;
		for (auto& child : children) {
			if (child->_WillRun(state)) {
				activeChildIndices.push_back(index);
			}
			index++;
		}
		if (activeChildIndices.size() > 0) {
			currentChild = activeChildIndices[Util::sharedRandom().nextInt(activeChildIndices.size())];
			return children[currentChild].get();
		}
	}
	return nullptr;
}
