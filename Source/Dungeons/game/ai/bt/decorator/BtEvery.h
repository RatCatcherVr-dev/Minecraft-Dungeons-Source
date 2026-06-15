#pragma once

#include "BtCached.h"
#include "game/ai/behavior/NodeFactory.h"

class UBtEvery : public UBtCached {
public:
	UBtEvery(bt::Duration);
protected:
	bool ShouldRunChild(bt::StateRef) override;

	bt::Duration interval;
	bt::TimeStamp next;
};


template <typename ...Tasks>
Unique<UBtEvery> every(bt::Duration interval, Tasks&&... tasks) {
	return meta(make_unique<UBtEvery>(interval), std::forward<Tasks>(tasks)...);
}
