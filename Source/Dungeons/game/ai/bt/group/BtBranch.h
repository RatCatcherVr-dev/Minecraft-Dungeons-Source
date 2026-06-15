#pragma once

#include "BtGroup.h"

class UBtBranch : public UBtGroup {
	using Super = UBtGroup;
public:
	UBtBranch();
protected:
	bool OnWillRun(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
	void OnStop(bt::StateRef) override;

	virtual UBtNode* OnWillRunGetBranch(bt::StateRef) = 0;
	static UBtNode* RunnableOrNull(bt::StateRef, UBtNode&);
private:
	UBtNode* next = nullptr;
	UBtNode* curr = nullptr;
};
