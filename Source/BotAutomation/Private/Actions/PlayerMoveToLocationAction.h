#pragma once

#include "PlayerBotActionBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "lovika/world/level/terrain/Terrain.h"

class ABasePlayerController;

class BOTAUTOMATION_API PlayerMoveToLocationAction : public PlayerBotActionBase
{
private:
	typedef PlayerBotActionBase Super;
public:
	PlayerMoveToLocationAction(FString Name, FVector Location, bool UsePathFinding = true, float AcceptanceRadius = -1.f)
		: Super(60.0, Name)
		, Location(Location)
		, UsePathFinding(UsePathFinding)
		, AcceptanceRadius(AcceptanceRadius)
	{
	}

	void StartAction() override;
	void StopAction() override;
	void DebugDraw() override;

protected:
	bool ExecuteAction(float DeltaSeconds) override;

	template<typename MoveMethod>
	bool ExecuteMove(MoveMethod moveMethod) {
		if (auto PlayerController = GetBasePlayerController())
		{
			if (const auto movement = PlayerController->GetPawn()->FindComponentByClass<UCharacterMovementComponent>()) {
				if (movement->IsFalling()) {
					return true;
				}
			}

			const auto pathFollowing = PlayerController->GetPathFollowingComponent();
			if (pathFollowing && (!IsFollowingPath(pathFollowing) || bMovedWithoutPathfinding))
			{
				auto result = moveMethod(UsePathFinding, AcceptanceRadius, false);
				bIsPartialPath = false;
				if (result == EPathFollowingRequestResult::Failed) {
					if (HasReached(pathFollowing, Location)) {
						EndAction(EPlayerBotActionResult::Success);
						return false;
					}

					result = moveMethod(UsePathFinding, AcceptanceRadius, true);
					bIsPartialPath = true;
				}

				switch (result)
				{
				case EPathFollowingRequestResult::RequestSuccessful:
					bMovedWithoutPathfinding = false;
					break;
				case EPathFollowingRequestResult::AlreadyAtGoal:
					EndAction(EPlayerBotActionResult::Success);
					return false;
				case EPathFollowingRequestResult::Failed:
					if (HasReached(pathFollowing, Location)) {
						EndAction(EPlayerBotActionResult::Success);
					}
					else {
						EndAction(EPlayerBotActionResult::Error);
					}
					return false;
				}

				if (bIsPartialPath && !IsFollowingPath(pathFollowing)) {
					if (PlayerAutomator->IsTerrainReachable(Location)) {
						bMovedWithoutPathfinding = true;
						PlayerAutomator->StopMovement();
						PlayerAutomator->DodgeInDirectionOf(Location);
					}
				}
			}
		}

		return true;
	}

	FVector Location;
	bool UsePathFinding;
	float AcceptanceRadius;

	bool IsFollowingPath(UPathFollowingComponent* PathFollowingComponent);
	bool HasReached(UPathFollowingComponent* PathFollowingComponent, FVector Vector);

private:
	bool bMovedWithoutPathfinding;
	bool bIsPartialPath;
};
