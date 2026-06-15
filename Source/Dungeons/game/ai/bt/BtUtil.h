#pragma once

#include "BtTypes.h"
#include "group/BtBranch.h"

class UBtNode;

class UBtUseReturnValue : public UBtBranch {
	using Super = UBtBranch;
public:
	UBtUseReturnValue(const bt::Action1Bool&);
protected:
	UBtNode* OnWillRunGetBranch(bt::StateRef) override;
private:
	bt::Action1Bool store;
};

template <typename Task>
Unique<UBtNode> useReturnValue(const bt::Action1Bool& store, Task&& task) {
	return meta(make_unique<UBtUseReturnValue>(store), std::forward<Task>(task));
}
