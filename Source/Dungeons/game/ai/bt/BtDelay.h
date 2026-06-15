#pragma once
#include "game/ai/bt/group/BtGroup.h"
#include "game/ai/bt/BtTime.h"
#include "game/ai/bt/BtLeaf.h"

class UDelay: public UBtLeaf {
public:
	UDelay(bt::Duration seconds);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void OnStart(bt::StateRef) override;
	void OnStop(bt::StateRef) override;
private:
	bt::TimeStamp endTime;
	bt::Duration duration;
};

Unique<UDelay> delay(bt::Duration seconds);
