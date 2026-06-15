#pragma once

#include "PlayerBotActionBase.h"

enum class EPlayerOnFunctionType
{
	MeleeAttack,
	RangedAttack,
	ActivateHealthSlot,
	ActivateSlot
};

class BOTAUTOMATION_API PlayerOnFunctionAction : public PlayerBotActionBase
{
private:
	typedef PlayerBotActionBase Super;
public:
	PlayerOnFunctionAction(FString Name, EPlayerOnFunctionType Action, int Index = 0)
		: Super(2, Name)
		, Action(Action)
		, Index(Index)
	{
	}

	void StopAction() override;
protected:
	bool ExecuteAction(float DeltaSeconds) override;
	void ResetAction();
private:
	EPlayerOnFunctionType Action;
	int Index;

	bool bPressed = false;


	bool ProcessAction();

	bool MeleeAttackAction();
	bool RangedAttackAction();
	bool ActivateHealthSlotAction();
	bool ActivateSlotAction();
};
