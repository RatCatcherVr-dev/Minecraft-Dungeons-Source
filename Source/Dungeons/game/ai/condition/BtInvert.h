#pragma once

#include "game/ai/bt/group/BtGroup.h"
#include "game/ai/behavior/NodeFactory.h"

class UBtInvert : public UBtGroup {
protected:
	bool OnWillRun(bt::StateRef) override;
};


template <typename Condition>
Unique<UBtInvert> invert(Condition&& condition) {
	return createGroup<UBtInvert>(std::forward<Condition>(condition));
}
