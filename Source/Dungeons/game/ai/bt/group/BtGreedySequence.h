#pragma once

#include "BtGroup.h"

class UBtGreedySequence : public UBtGroup {
	using Super = UBtGroup;
public:
	UBtGreedySequence();
protected:
	bool OnWillRun(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
	void OnStop(bt::StateRef) override;
private:
	int nextCount = 0;
	int currCount = 0;
};
