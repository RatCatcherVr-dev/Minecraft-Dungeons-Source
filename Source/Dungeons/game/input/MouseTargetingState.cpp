#include "Dungeons.h"
#include "MouseTargetingState.h"
#include "game/actor/character/player/BasePlayerController.h"

namespace input {

	MouseInputStateTypeId MouseTargetingState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) {
		controller.StopMovement();
		controller.GetControlledPlayerCharacter()->GetCharacterMovement()->StopMovementImmediately();
		targetController.SetTargetType(TargetController::ETargetType::TargetOnly);

		return UndefinedState;
	}

	MouseInputStateTypeId MouseTargetingState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if( controller.GetGamepadActive() ) {
			if (mouseState.MoveAxis.Active) {
				auto player = controller.GetControlledPlayerCharacter();
				auto destination = controller.ProjectInputAxesFromPlayer(FVector(mouseState.MoveAxis.X, mouseState.MoveAxis.Y, 0.0f));
				player->RotatePlayerTowardsLocation(destination);
			}
		} else {
			controller.RotatePlayerTowardsCursor();
		}

		return UndefinedState;
	}

	MouseInputStateTypeId MouseTargetingState::OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (!mouseState.MoveButton.Pressed) {
			controller.StopMovement();
		} else {
			return GetId<MouseButtonEntryState>(); 
		}

		return UndefinedState;
	}

	MouseInputStateTypeId MouseTargetingState::OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (mouseState.RangedButton.Pressed) {
			return GetId<MouseButtonEntryState>(); 
		}

		return UndefinedState;
	}

	MouseInputStateTypeId MouseTargetingState::OnMeleeButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (mouseState.MeleeButton.Pressed) {
			return GetId<MouseButtonEntryState>(); 
		}

		return UndefinedState;
	}

	void MouseTargetingState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		controller.StopMovement();
		targetController.SetTargetType(TargetController::ETargetType::All);

		if(auto player = controller.GetControlledPlayerCharacter()) {
			player->CancelAllActions();
		}
	}

}