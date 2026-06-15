#pragma once

#include "game/ai/bt/group/BtGroup.h"

class UBtCached : public UBtGroup {
protected:
	bool OnWillRun(bt::StateRef) override;
	void OnTick(bt::StateRef) override;

	virtual bool ShouldRunChild(bt::StateRef) = 0;
private:
	bool lastWillRun = false;
	bool shouldRunChild = false;
};
