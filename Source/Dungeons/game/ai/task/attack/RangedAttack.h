#pragma once

#include "game/ai/bt/BtLeaf.h"
#include "game/ai/bt/BtTypes.h"
#include "util/IntChanged.h"

class AActor;

class URangedAttack : public UBtLeaf {
public:
	URangedAttack();
	URangedAttack(FName ComponentTag);
	URangedAttack(const bt::Provider<AActor*>&, int maxAttacksToIssue = 1, FName ComponentTag = "");
	URangedAttack(const bt::Provider<AActor*>&, const bt::Provider<FVector>&, int maxAttacksToIssue = 1, FName ComponentTag = "");
protected:
	bool OnCanRun(bt::StateRef) override;
	bool OnCanContinue(bt::StateRef) override;

	void Init(bt::StateRef) override;
	void OnStart(bt::StateRef) override;
	void OnTick(bt::StateRef) override;
	void OnStop(bt::StateRef) override;
private:
	void _Attack(bt::StateRef);	

	bool freeAim = false;

//struct Memory {
	bt::Provider<AActor*> targetProvider;
	bt::Provider<FVector> originProvider;
	bt::TimeStamp postAttackTime;
	IntChanged currentAttackCounter;
	FName componentTag;
	class URangedAttackComponent* attackComponent;
	class UBehaviorOptionsComponent* optionsComponent;
	//bool isDone = false;
	int maxAttacks;
	int attacksIssued;
	float startTime;
//} mem;
};
