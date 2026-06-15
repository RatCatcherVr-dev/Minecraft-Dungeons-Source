#pragma once

#include "BtGroup.h"

class UBtChain : public UBtGroup {
	using Super = UBtGroup;

public:
	UBtChain();

protected:
	bool OnWillRun(bt::StateRef) override;
	void OnTick(bt::StateRef) override;

private:
	int currentChildIndex = 0;
	int lastChildIndex = 0;
};