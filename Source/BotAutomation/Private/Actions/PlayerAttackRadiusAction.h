#pragma once

#include "PlayerAttackMobAction.h"
#include "game/team/TeamName.h"

class BOTAUTOMATION_API PlayerAttackRadiusAction : public PlayerAttackMobAction
{
private:
	typedef PlayerAttackMobAction Super;
public:
	PlayerAttackRadiusAction(FString Name, FVector Location, ETeamName AttackableTeam, float AttackRadius, bool AllowRanged = false)
		: Super(Name, nullptr, AllowRanged)
		, Location(Location)
		, AttackableTeam(AttackableTeam)
		, AttackRadius(AttackRadius)
	{
	}

	void StopAction() override;
	void StartAction() override;
	void DebugDraw() override;

protected:
	bool ExecuteAction(float DeltaSeconds) override;
	void SetTarget(ABasePlayerController* PlayerController, TWeakObjectPtr<ABaseCharacter> Target) override;

private:
	FVector Location;
	ETeamName AttackableTeam;
	float AttackRadius;

	bool ShouldTargetNewActor(const APlayerCharacter* player, TWeakObjectPtr<ABaseCharacter> target, float radius);
	ABaseCharacter* GetClosestCharacter(AActor* source, float radius, int sourceActorID = INDEX_NONE);
};
