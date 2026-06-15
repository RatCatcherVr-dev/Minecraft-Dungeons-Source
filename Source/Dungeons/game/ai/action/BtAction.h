#pragma once

#include "game/ai/bt/BtNode.h"
#include "game/ai/bt/BtTypes.h"
#include "CommonTypes.h"
#include <functional>
#include <vector>

class UBtAction: public UBtNode {
public:
	UBtAction(const bt::Action&);
	UBtAction(std::initializer_list<bt::Action>);
protected:
	bool OnWillRun(bt::StateRef) final { return true; }
	void CallActions(bt::StateRef);

	std::vector<bt::Action> actions;
};


class UBtStartAction : public UBtAction {
	using Super = UBtAction;
public:
	using Super::Super;
protected:
	void OnStart(bt::StateRef state) {
		CallActions(state);
	}
};


class UBtTickAction : public UBtAction {
	using Super = UBtAction;
public:
	using Super::Super;
protected:
	void OnTick(bt::StateRef state) final {
		CallActions(state);
	}
};


class UBtStopAction : public UBtAction {
	using Super = UBtAction;
public:
	using Super::Super;
protected:
	void OnStop(bt::StateRef state) final {
		CallActions(state);
	}
};

//
// Factory
//
Unique<UBtNode> onStart(const bt::Action&);
Unique<UBtNode> onStart(std::initializer_list<bt::Action>);
Unique<UBtNode> onTick(const bt::Action&);
Unique<UBtNode> onTick(std::initializer_list<bt::Action>);
Unique<UBtNode> onStop(const bt::Action&);
Unique<UBtNode> onStop(std::initializer_list<bt::Action>);
