#pragma once

#include "BtBranch.h"

class UBtIfElse : public UBtBranch {
	using Super = UBtBranch;
public:
	UBtIfElse();
protected:
	UBtNode* OnWillRunGetBranch(bt::StateRef) override;
};
