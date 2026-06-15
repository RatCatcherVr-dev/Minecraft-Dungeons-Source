#include "Dungeons.h"
#include "BtChain.h"

UBtChain::UBtChain() {
	type = "chain";
}


bool UBtChain::OnWillRun(bt::StateRef state) {
	if (IsRunning()) {
		const auto& child = children[currentChildIndex];
		
		if (child->IsRunning()) {
			if (!child->_WillRun(state)) {				
				if (currentChildIndex < static_cast<int>(children.size()) - 1) {
					currentChildIndex++;
					return children[currentChildIndex]->_WillRun(state);
				} else {
					return false;
				}
			} else {
				return true;
			}
		} else {
			return child->_WillRun(state);
		}
	} else {
		currentChildIndex = 0;
		return children[currentChildIndex]->_WillRun(state);
	}
}

void UBtChain::OnTick(bt::StateRef state) {
	if (lastChildIndex != currentChildIndex && lastChildIndex >= 0 && lastChildIndex < children.size()) {		
		children[lastChildIndex]->Stop(state);
	}
	children[currentChildIndex]->_Tick(state);
	lastChildIndex = currentChildIndex;
}