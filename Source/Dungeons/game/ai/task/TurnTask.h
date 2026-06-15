#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/provider/Locators.h"

class UTurnTask : public UBtLeaf {
public:
	UTurnTask(
		const bt::locator::Provider& target,
		float rate,
		float toleranceDegrees = 20.f
	);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void OnTick(bt::StateRef) override;

private:
	bt::locator::Provider target;
	float rate;
	float tolerance;
};
