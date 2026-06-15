#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/provider/Actors.h"
#include "util/IntChanged.h"

class UMeleeAttack : public UBtLeaf {
public:
	UMeleeAttack(const bt::actor::Provider&, bool blind = false, int index = -1);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
	void OnStop(bt::StateRef) override;
private:
//struct Memory {
	bt::actor::Provider targetProvider;
	IntChanged currentAttackCounter;
	class UMeleeAttackComponent* attackComponent;
	bool isDone = false;
	bool blind = false;
	int index = -1;
//} mem;
};
