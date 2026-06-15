#pragma once

#include "BtBranch.h"

class UBtStartIfElse : public UBtBranch {
	using Super = UBtBranch;
public:
	UBtStartIfElse();
protected:
	
	void OnStop(bt::StateRef) override;

	UBtNode* OnWillRunGetBranch(bt::StateRef) override;
	
private:
	int cachedBranchIndex = -1;
};
