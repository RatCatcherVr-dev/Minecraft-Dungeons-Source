#include "BotAutomationPCH.h"
#include "PlayerMoveToLocationAction.h"

#include "game/actor/character/player/PlayerCharacter.h"

#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/actor/character/player/BasePlayerController.h"

#include "game/GameBP.h"
#include "game/Conversion.h"

void PlayerMoveToLocationAction::StartAction()
{
	Super::StartAction();

	PlayerAutomator->StopMovement();
}

void PlayerMoveToLocationAction::StopAction()
{
	if (auto PlayerCharacter = GetPlayerCharacter())
	{
		PlayerAutomator->StopMovement();
		PlayerCharacter->RotatePlayerTowardsLocation(Location);
		PlayerAutomator->MoveInDirectionOf(Location);
	}

	Super::StopAction();
}

bool PlayerMoveToLocationAction::ExecuteAction(float DeltaSeconds)
{
	return ExecuteMove([&](const bool bUsePathFinding, const float fAcceptanceRadius, const bool bAllowPartialPath) {
		return PlayerAutomator->MoveToLocation(Location, bUsePathFinding, fAcceptanceRadius, bAllowPartialPath);
		});
}

void PlayerMoveToLocationAction::DebugDraw()
{
	Super::DebugDraw();

#if !UE_BUILD_SHIPPING
	PlayerAutomator->DebugDrawPathFind();

	const auto world = PlayerAutomator->GetWorld();
	const auto player = GetPlayerCharacter();
	if (player) {
		const auto source = player->GetActorLocation();
		DrawDebugLine(world, source, Location, FColor::Emerald, false, -1, 0, 2.5f);
	}
#endif
}

bool PlayerMoveToLocationAction::IsFollowingPath(UPathFollowingComponent* PathFollowingComponent)
{
	const auto path = PathFollowingComponent->GetPath();
	if (!path.IsValid()) {
		return false;
	}

	if (PathFollowingComponent->GetStatus() == EPathFollowingStatus::Idle) {
		return false;
	}

	if (path->GetPathPoints().Num() > 0) {
		return !HasReached(PathFollowingComponent, path->GetPathPoints().Last());
	}

	return false;
}

bool PlayerMoveToLocationAction::HasReached(UPathFollowingComponent* PathFollowingComponent, FVector Vector)
{
	return PathFollowingComponent->HasReached(Vector, EPathFollowingReachMode::OverlapAgentAndGoal, AcceptanceRadius);
}
