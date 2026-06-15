#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/provider/Locators.h"
#include "util/FloatRange.h"

class UIsInRange : public UBtLeaf {
public:
	UIsInRange(const bt::locator::Provider&, FloatRange distanceRange);
	UIsInRange(const bt::locator::Provider&, FloatRange enterDistanceRange, FloatRange exitDistanceRange);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;
private:
	bt::Pred inEnterRange, inExitRange;
};


Unique<UIsInRange> isInRange(const bt::locator::Provider&, FloatRange distanceRange);
Unique<UIsInRange> isInRange(const bt::locator::Provider&, FloatRange enterDistanceRange, FloatRange exitDistanceRange);
