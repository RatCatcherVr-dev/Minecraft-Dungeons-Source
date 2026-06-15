#pragma once

#include "BtGroup.h"

class UBtSequence : public UBtGroup {
	using Super = UBtGroup;
public:
	UBtSequence();
protected:
	bool OnWillRun(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
};
