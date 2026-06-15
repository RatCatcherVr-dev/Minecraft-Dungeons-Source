#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/component/TurnComponent.h"
#include "ReturnTask.h"

class UReturnTask : public UBtLeaf {
public:
	UReturnTask(UTurnComponent*, bt::Provider<float> angleProvider);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void OnStart(bt::StateRef) override;
private:
	TWeakObjectPtr<UTurnComponent> component;
	bt::Provider<float> angleProvider;
	float targetAngle;
};
