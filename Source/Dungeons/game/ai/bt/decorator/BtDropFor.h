#pragma once

#include "BtCached.h"
#include "game/ai/behavior/NodeFactory.h"

class UBtDropFor : public UBtCached {
	using Super = UBtCached;
public:
	UBtDropFor(bt::Duration);
protected:
	bool ShouldRunChild(bt::StateRef) override;
	void OnStop(bt::StateRef) override;

	bt::Duration delay;
	bt::TimeStamp next;
	bool initialised = false;
};


template <typename ...Tasks>
Unique<UBtDropFor> dropFor(bt::Duration delay, Tasks&&... tasks) {
	return meta(make_unique<UBtDropFor>(delay), std::forward<Tasks>(tasks)...);
}
