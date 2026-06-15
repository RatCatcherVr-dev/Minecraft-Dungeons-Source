#pragma once

#include "BtBranch.h"

class UBtRandomSelector : public UBtBranch {
	using Super = UBtBranch;
public:
	UBtRandomSelector();
protected:
	UBtNode* OnWillRunGetBranch(bt::StateRef) override;
private:
	int currentChild;
};
