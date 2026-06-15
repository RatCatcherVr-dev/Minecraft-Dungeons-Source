#pragma once

#include "game/ai/bt/BtLeaf.h"

class UHealTask : public UBtLeaf {
public:
	UHealTask(
		UAnimSequenceBase* sequence,
		bt::Duration animationDuration,
		bt::Duration summonDelay,
		bt::Duration summonCooldown,
		float amount
	);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void OnStart(bt::StateRef) override;
	bool TryHeal(bt::StateRef state);

private:
	UAnimSequenceBase* sequence;
	bt::Duration animationDuration;
	bt::Duration healDelay;
	bt::Duration healCooldown;
	float amount;
	bt::TimeStamp healTime;
	bt::TimeStamp animationEndTime;
	bt::TimeStamp earliestStartTime;
	bool healed;
};
