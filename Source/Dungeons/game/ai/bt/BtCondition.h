#pragma once

#include "group/BtGroup.h"

class UBtCondition : public UBtGroup {
	using Super = UBtGroup;
public:
	UBtCondition();
protected:
	bool OnWillRun(bt::StateRef) override;
};
