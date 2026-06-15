#pragma once
#include "game/ai/bt/group/BtBranch.h"

class UBtRoot : public UBtBranch {
	using Super = UBtBranch;
public:
	UBtRoot();
	void SetLatentTask(bt::StateRef, UBtNode*);
	void OnStop(bt::StateRef);
protected:
	void StopLatentTask(bt::StateRef);
	void FillData(FBtEvalTraversalState&) override;
	UBtNode* OnWillRunGetBranch(bt::StateRef) override;

	UBtNode* latentTask = nullptr;
};
