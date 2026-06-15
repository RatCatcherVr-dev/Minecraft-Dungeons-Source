#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/bt/BtTypes.h"
#include "game/ai/provider/Actors.h"
#include "util/IntChanged.h"

class UAoeAttack : public UBtLeaf {
public:
	UAoeAttack(bool blind = false);
	UAoeAttack(FName ComponentTag, bool blind = false);
	UAoeAttack(const bt::Provider<AActor*>& provider, FName ComponentTag = "" , bool blind = false);
	UAoeAttack(const bt::Provider<AActor*>&, int maxAttacksToIssue = 1);
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void Init(bt::StateRef) override;
	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
	void OnStop(bt::StateRef) override;
private:
//struct Memory {
	bt::Provider<AActor*> AttackProvider;
	IntChanged currentAttackCounter;
	class UAoeAttackComponent* attackComponent;
	bool isDone = false;
	bool blind = false;
	FName ComponentTag;
	//bt::TimeStamp startTime;
	float startTime;
	bt::TimeStamp endTime;
//} mem;
};
