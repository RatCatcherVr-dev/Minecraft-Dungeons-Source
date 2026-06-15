#include "BotAutomationPCH.h"

#include "game/actor/character/player/PlayerCharacter.h"

#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/actor/character/player/BasePlayerController.h"

PlayerBotActionBase::PlayerBotActionBase()
	: PlayerBotActionBase(0.0)
{
}

PlayerBotActionBase::PlayerBotActionBase(float ActionRuntime)
	: PlayerBotActionBase(ActionRuntime, FString())
{
}

PlayerBotActionBase::PlayerBotActionBase(float ActionRuntime, FString ActionName)
	: RuntimeRemaining(ActionRuntime)
	, OrginalRuntime(ActionRuntime)
	, BotActionName(ActionName)
{
	Status = EPlayerBotActionResult::Stopped;
}

PlayerBotActionBase::~PlayerBotActionBase()
{
}

void PlayerBotActionBase::SetPlayerAutomator(APlayerAutomator* Automator)
{
	PlayerAutomator = Automator;
}

void PlayerBotActionBase::RemovePlayerAutomator()
{
	PlayerAutomator = nullptr;
}

void PlayerBotActionBase::StartAction()
{
	Status = EPlayerBotActionResult::InProgress;
	bLogicStarted = true;
	bLogicRunning = true;
}

void PlayerBotActionBase::UpdateAction(float DeltaSeconds)
{
	if (bLogicRunning)
	{
		Status = EPlayerBotActionResult::InProgress;

		const bool continueAction = ExecuteAction(DeltaSeconds);

		RuntimeRemaining -= DeltaSeconds;

		if (continueAction && RuntimeRemaining < 0.f) {
			EndAction(EPlayerBotActionResult::ErrorTimedOut);
		}

		if (!continueAction) {
			bLogicRunning = false;

			if (Status == EPlayerBotActionResult::InProgress) {
				Status = EPlayerBotActionResult::Stopped;
			}
		}
	}
}

bool PlayerBotActionBase::ExecuteAction(float DeltaSeconds)
{
	return true;
}

void PlayerBotActionBase::StopAction()
{
	if (Status == EPlayerBotActionResult::InProgress) {
		Status = EPlayerBotActionResult::Stopped;
	}

	bLogicRunning = false;
}

void PlayerBotActionBase::ResetAction()
{
	StopAction();
	bLogicStarted = false;
	RuntimeRemaining = OrginalRuntime;
}

void PlayerBotActionBase::SetStatus(EPlayerBotActionResult status)
{
	Status = status;
}

void PlayerBotActionBase::EndAction(EPlayerBotActionResult status)
{
	Status = status;
	RuntimeRemaining = -1.0;
	bLogicRunning = false;
}

APlayerControllerBase* PlayerBotActionBase::GetPlayerControllerBase()
{
	if (PlayerAutomator) {
		return PlayerAutomator->GetAttachedController();
	}
	return nullptr;
}

ABasePlayerController* PlayerBotActionBase::GetBasePlayerController()
{
	return Cast<ABasePlayerController>(GetPlayerControllerBase());
}

APlayerCharacter* PlayerBotActionBase::GetPlayerCharacter(class APlayerControllerBase* Controller)
{
	if (!Controller) {
		Controller = GetBasePlayerController();
	}

	if (Controller) {
		return Cast<APlayerCharacter>(Controller->GetCharacter());
	}

	return nullptr;
}

FString PlayerBotActionBase::GetStatusText() const
{
	auto ResultString = TEXT("Unknown");
	switch (Status)
	{
	case EPlayerBotActionResult::Stopped:
		ResultString = TEXT("Stopped");
		break;

	case EPlayerBotActionResult::Success:
		ResultString = TEXT("Success");
		break;

	case EPlayerBotActionResult::InProgress:
		ResultString = TEXT("InProgress");
		break;

	case EPlayerBotActionResult::ErrorTimedOut:
		ResultString = TEXT("Error.TimedOut");
		break;

	default:
		ResultString = TEXT("Error");
		break;
	}

	return FString(ResultString);
}


FString PlayerBotActionBase::UserFriendlyName()
{
	if (!BotActionName.IsEmpty()) {
		return BotActionName;
	}
	return FString(TEXT("Unknown Name"));
}

void PlayerBotActionBase::DebugDraw()
{
}
