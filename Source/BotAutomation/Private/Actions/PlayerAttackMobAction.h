#pragma once

#include "PlayerBotActionBase.h"
#include <WeakObjectPtrTemplates.h>

class ABaseCharacter;
class ABasePlayerController;
class APlayerCharacter;
class UMeleeAttackComponent;
class UAutoAimRangedAttackComponent;

class BOTAUTOMATION_API PlayerAttackMobAction : public PlayerBotActionBase
{
private:
	typedef PlayerBotActionBase Super;
public:
	PlayerAttackMobAction(FString Name, TWeakObjectPtr<ABaseCharacter> Target, bool AllowRanged = false)
		: Super(60.0, Name)
		, CurrentTarget(Target)
		, bAllowRanged(AllowRanged)
	{
	}

	virtual void StopAction() override;
	virtual void StartAction() override;
	virtual void DebugDraw() override;

protected:
	bool ExecuteAction(float DeltaSeconds) override;

	virtual void MeleeAttack(ABasePlayerController* PlayerController, APlayerCharacter* Character, UMeleeAttackComponent* MeleeComponent, ABaseCharacter* Target);
	virtual void RangedAttack(ABasePlayerController* PlayerController, APlayerCharacter* Character, UAutoAimRangedAttackComponent* RangedComponent, ABaseCharacter* Target, float Range = 0);

	TWeakObjectPtr<ABaseCharacter> CurrentTarget;

	bool bHasAttacked;
	bool bHasMeleePressed;
	bool bHasRangedPressed;
	bool bAllowRanged;

	bool IsValidTarget(const APlayerCharacter* player, TWeakObjectPtr<ABaseCharacter> target);
	bool IsTargetable(const APlayerCharacter* player, TWeakObjectPtr<ABaseCharacter> target);
	virtual void SetTarget(ABasePlayerController* PlayerController, TWeakObjectPtr<ABaseCharacter> Target);
	bool IsTargetInLOS(APlayerCharacter* Character, TWeakObjectPtr<ABaseCharacter> Target);

	void ReleasePlayerAttackButton(ABasePlayerController* PlayerController);
};
