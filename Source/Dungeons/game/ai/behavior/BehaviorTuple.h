#pragma once

#include "CommonTypes.h"
#include "game/component/BehaviorComponent.h"

class UBtNode;

namespace bt {

struct BehaviorTuple {
	BehaviorTuple(Unique<UBtNode> actions = {}, Unique<UBtNode> targets = {})
		: actions(std::move(actions))
		, targets(std::move(targets)) {
	}

	explicit operator bool() const {
		return actions || targets;
	}

	void moveTo(UBehaviorComponent*);

	Unique<UBtNode> actions;
	Unique<UBtNode> targets;
};

}
