#pragma once
#include "BtBranch.h"

enum class ERotateOn {
	Start,
	Tick
};

class UBtRotate : public UBtBranch {
	using Super = UBtBranch;
public:
	UBtRotate(ERotateOn);
protected:
	void OnTick(bt::StateRef) override;
	void OnStop(bt::StateRef) override;

	UBtNode* OnWillRunGetBranch(bt::StateRef) override;
private:
	ERotateOn rotateOn;
	int index = 0;
};
