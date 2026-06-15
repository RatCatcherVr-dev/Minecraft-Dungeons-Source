#pragma once

#include "PlayerMoveToLocationAction.h"

class BOTAUTOMATION_API PlayerMoveToActorAction : public PlayerMoveToLocationAction
{
private:
	typedef PlayerMoveToLocationAction Super;
public:
	PlayerMoveToActorAction(FString Name, AActor* Target, bool UsePathFinding = true, float AcceptanceRadius = -1.f)
		: Super(Name, FVector::ZeroVector, UsePathFinding, AcceptanceRadius)
		, Target(Target)
	{
	}

	void StartAction() override;
	void StopAction() override;

protected:
	bool ExecuteAction(float DeltaSeconds) override;

private:
	AActor* Target;
	bool HasReached(UPathFollowingComponent* PathFollowingComponent, AActor* Actor);
	void UpdateLocationFromTarget();
};
