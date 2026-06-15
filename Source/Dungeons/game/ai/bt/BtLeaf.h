#pragma once

#include "BtNode.h"

class UBtLeaf : public UBtNode {
protected:
	UBtLeaf();
	virtual bool OnCanRun(bt::StateRef);
	virtual bool OnCanContinue(bt::StateRef);
	virtual void Init(bt::StateRef) {}

	bool OnWillRun(bt::StateRef) final;
	void OnInitialise(bt::StateRef) final;

private:
	bool shouldInit = true;
};
