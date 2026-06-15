#pragma once
#include "game/ai/bt/group/BtGroup.h"

class UBtUninterruptible : public UBtGroup {
public:
	UBtUninterruptible();
protected:
	bool OnWillRun(bt::StateRef) override;
	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
};
