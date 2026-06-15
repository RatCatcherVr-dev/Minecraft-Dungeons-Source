#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/provider/Locations.h"
#include "game/ai/provider/move.h"
#include "AITypes.h"

class UMoveTo: public UBtLeaf {
public:
	UMoveTo(const bt::move::Provider&);
	UMoveTo(const bt::location::Provider&, bool, const FAIMoveRequest& = bt::move::defaultRequest());

protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
	void OnStop(bt::StateRef) override;

private:
	enum class Type { Location, Move };

	Type type;
	bt::location::Provider locationProvider;
	bt::move::Provider moveRequestProvider;
	bool continuously;
	FAIMoveRequest moveRequest;
	FPathFindingQuery currentQuery;
};
