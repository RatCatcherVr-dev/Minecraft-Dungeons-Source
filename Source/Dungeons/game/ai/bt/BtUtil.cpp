#include "Dungeons.h"
#include "BtUtil.h"

UBtUseReturnValue::UBtUseReturnValue(const bt::Action1Bool& store)
	: store(store) {
}

UBtNode* UBtUseReturnValue::OnWillRunGetBranch(bt::StateRef state) {
	auto child = children[0].get();
	bool wantRun = child->_WillRun(state);
	store(state, wantRun);
	return wantRun ? child : nullptr;
}
