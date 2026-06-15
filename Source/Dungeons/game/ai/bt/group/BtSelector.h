#pragma once

#include "BtBranch.h"

class UBtSelector : public UBtBranch {
	using Super = UBtBranch;
public:
	UBtSelector();
protected:
	UBtNode* OnWillRunGetBranch(bt::StateRef) override;
};
