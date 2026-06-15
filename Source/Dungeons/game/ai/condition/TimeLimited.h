#pragma once

#include "game/ai/bt/BtTime.h"
#include "game/ai/bt/BtLeaf.h"
#include "CommonTypes.h"

enum class ETimeLimit {
	MinTimeBetweenStarts,
	MinTimeBetweenStopAndStart,
	MaxTimeBetweenStartAndStop
};

class UTimeLimited : public UBtLeaf {
public:
	UTimeLimited(bt::Duration, ETimeLimit);
protected:
	void OnStart(bt::StateRef) override;
	void OnStop(bt::StateRef) override;
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;
private:
	const bt::Duration interval;
	const ETimeLimit betweenWhat;
	bt::TimeStamp nextTimeLimit;
};


template <typename ...Args>
Unique<UTimeLimited> timeLimited(Args&&... args) {
	return make_unique<UTimeLimited>(std::forward<Args>(args)...);
}
Unique<UTimeLimited> minTimeBetweenStopAndStart(bt::Duration);
Unique<UTimeLimited> minTimeBetweenStarts(bt::Duration);
Unique<UTimeLimited> maxRunTime(bt::Duration);
