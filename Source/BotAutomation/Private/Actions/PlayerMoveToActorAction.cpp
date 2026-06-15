#include "BotAutomationPCH.h"
#include "PlayerMoveToActorAction.h"

#include "game/actor/character/player/PlayerCharacter.h"

#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/actor/character/player/BasePlayerController.h"

#include "game/GameBP.h"
#include "game/Conversion.h"

void PlayerMoveToActorAction::StartAction()
{
	UpdateLocationFromTarget();
	Super::StartAction();
}

void PlayerMoveToActorAction::StopAction()
{
	UpdateLocationFromTarget();
	Super::StopAction();
}

bool PlayerMoveToActorAction::ExecuteAction(float DeltaSeconds)
{
	if (!Target || Target->IsPendingKillOrUnreachable()) {
		EndAction(EPlayerBotActionResult::Error);
		return false;
	}
	else {
		Location = Target->GetActorLocation();
	}

	return ExecuteMove([&](const bool bUsePathFinding, const float fAcceptanceRadius, const bool bAllowPartialPath) {
		return PlayerAutomator->MoveToActor(Target, bUsePathFinding, fAcceptanceRadius, bAllowPartialPath);
		});
}

bool PlayerMoveToActorAction::HasReached(UPathFollowingComponent* PathFollowingComponent, AActor* Actor)
{
	return PathFollowingComponent->HasReached(*Actor, EPathFollowingReachMode::OverlapAgentAndGoal, AcceptanceRadius);
}

void PlayerMoveToActorAction::UpdateLocationFromTarget()
{
	if (Target) {
		Location = Target->GetActorLocation();
	}
}