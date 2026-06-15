#include "PlayerOnFunctionAction.h"

#include "game/actor/character/player/BasePlayerController.h"

bool PlayerOnFunctionAction::ExecuteAction(float DeltaSeconds)
{
	const bool continueAction = ProcessAction();
	if (!continueAction) {
		EndAction(EPlayerBotActionResult::Success);
	}
	return continueAction;
}

bool PlayerOnFunctionAction::ProcessAction()
{
	bool result = false;

	switch (Action)
	{
	case EPlayerOnFunctionType::MeleeAttack:
		result = MeleeAttackAction();
		break;
	case EPlayerOnFunctionType::RangedAttack:
		result = RangedAttackAction();
		break;
	case EPlayerOnFunctionType::ActivateHealthSlot:
		result = ActivateHealthSlotAction();
		break;
	case EPlayerOnFunctionType::ActivateSlot:
		result = ActivateSlotAction();
		break;
	default:
		EndAction(EPlayerBotActionResult::Error);
		break;
	}

	return result;
}

bool PlayerOnFunctionAction::MeleeAttackAction()
{
	if (auto PlayerController = GetBasePlayerController())
	{
		if (!bPressed) {
			PlayerController->OnMeleeAttackButtonPressed();
			bPressed = true;
		}
		else {
			PlayerController->OnRootPlayerGamepadReleased();
			PlayerController->OnMeleeAttackButtonReleased();
			bPressed = false;
		}
		return bPressed;
	}

	return false;
}

bool PlayerOnFunctionAction::RangedAttackAction()
{
	if (auto PlayerController = GetBasePlayerController())
	{
		if (!bPressed) {
			PlayerController->OnRangedAttackButtonGamepadPressed();
			bPressed = true;
		}
		else {
			PlayerController->OnRootPlayerGamepadReleased();
			PlayerController->OnRangedAttackButtonGamepadReleased();
			bPressed = false;
		}
		return bPressed;
	}

	return false;
}

bool PlayerOnFunctionAction::ActivateHealthSlotAction()
{
	if (auto playerController = GetBasePlayerController())
	{
		playerController->SetWantsToActivateSlot(ESlotType::HealthPotion, Index, !bPressed);
		bPressed = !bPressed;
		return bPressed;
	}

	return false;

}

bool PlayerOnFunctionAction::ActivateSlotAction()
{
	if (auto playerController = GetBasePlayerController())
	{
		if (Index >= 0 && Index <= 2) {
			playerController->SetWantsToActivateSlot(ESlotType::ActivePermanent, Index, !bPressed);
			bPressed = !bPressed;
			return bPressed;
		}
	}

	return false;
}

void PlayerOnFunctionAction::ResetAction()
{
	Super::ResetAction();
	if (bPressed) {
		ProcessAction();
		bPressed = false;
	}
}

void PlayerOnFunctionAction::StopAction()
{
	if (bPressed) {
		ProcessAction();
	}

	Super::StopAction();
}