#pragma once

#include "BtGroup.h"

class UBtParallel : public UBtGroup {
	using Super = UBtGroup;
public:
	UBtParallel();
protected:
	bool OnWillRun(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
};
