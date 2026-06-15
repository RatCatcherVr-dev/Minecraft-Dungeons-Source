#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/provider/Actors.h"
#include "util/IntChanged.h"

class UEnchant : public UBtLeaf {
public:
	UEnchant(const bt::actor::Provider&);
	UEnchant(const bt::actor::MultiProvider&);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void Init(bt::StateRef) override;
	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
	void OnStop(bt::StateRef) override;
private:
//struct Memory {
	bt::actor::MultiProvider targetProvider;
	IntChanged currentAttackCounter;
	class UGrowAttackComponent* enchantComponent;
	bool isDone = false;
//} mem;
};
