#pragma once

#include "game/ai/bt/BtLeaf.h"

class UPlayAnimation: public UBtLeaf {
public:
	UPlayAnimation(UAnimSequenceBase*, bool loopForever = false, bool blend = true, bool fire = false);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void OnStart(bt::StateRef) override;
	void OnStop(bt::StateRef) override;
private:
	UAnimSequenceBase* sequence;
	bool loopForever;
	bool blend;
	bool fire;
	bt::TimeStamp endTime;
};

Unique<UPlayAnimation> playAnimation(UAnimSequenceBase*, bool loopForever = false, bool blend = true, bool fire = false);
