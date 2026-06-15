#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "AITypes.h"

#include "game/ai/behavior/BehaviorTuple.h"
#include "game/ai/provider/Move.h"

class UBehaviorOptionsComponent;

namespace actor { class Provider; }

class UBtPiggyBankFlee : public UBtLeaf {
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void OnStart(bt::StateRef) override;
	void OnStop(bt::StateRef) override;

	virtual FAIMoveRequest GetMoveRequest(bt::StateRef) = 0;
private:
	FAIMoveRequest moveRequest;
};


class UPiggyBankFlee : public UBtPiggyBankFlee {
public:
	UPiggyBankFlee(const ::bt::move::Provider&);
protected:
	FAIMoveRequest GetMoveRequest(bt::StateRef) override;
private:
	bt::move::Provider moveRequestProvider;
};


namespace bt { namespace move {
	using Provider = bt::Provider<FAIMoveRequest>;
	Provider PiggyBankFleeFromPlayerToLocation(const actor::Provider&, float radius = 75.0f);
	}
}


namespace bt {namespace behavior { namespace entities {

BehaviorTuple createPiggyBank(class AMobCharacter&, const UBehaviorOptionsComponent&);

}}}
