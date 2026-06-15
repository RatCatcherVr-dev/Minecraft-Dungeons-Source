#include "Dungeons.h"
#include "MouseDynamicRootState.h"
#include "MouseButtonEntryState.h"
#include "MouseRangedState.h"
#include "game/actor/character/player/BasePlayerController.h"

namespace input {

	input::MouseInputStateTypeId MouseDynamicRootState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState)
	{
		return UndefinedState;
	}

	void MouseDynamicRootState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{

	}

	input::MouseInputStateTypeId MouseDynamicRootState::OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		if (mouseState.RangedButton.Pressed)
		{
			return GetId<MouseRangedState>();
		}
		return UndefinedState;
	}

	input::MouseInputStateTypeId MouseDynamicRootState::OnMoveAxis(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		if (mouseState.MoveAxis.Active)
		{
			// If we're not about to fire we need to allow the player to control their character's rotation.
			auto destination = controller.ProjectInputAxesFromPlayer(FVector(mouseState.MoveAxis.X, mouseState.MoveAxis.Y, 0.0f));
			auto player = controller.GetControlledPlayerCharacter();
			player->RotatePlayerTowardsLocation(destination);
		}

		return UndefinedState;
	}

	input::MouseInputStateTypeId MouseDynamicRootState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		if (!mouseState.RootButton.Pressed)
		{
			return GetId<MouseButtonEntryState>();
		}
		
		return UndefinedState;
	}
}