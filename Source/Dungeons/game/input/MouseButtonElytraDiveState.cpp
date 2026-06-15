#include "Dungeons.h"
#include "MouseButtonElytraDiveState.h"
#include "TargetController.h"
#include "MouseButtonEntryState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/movement/MovementFlyingCommon.h"
#include "game/component/ElytraComponent.h"

namespace input {

	MouseInputStateTypeId MouseButtonElytraDiveState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) {
		UPlayerCharacterMovementComponent* characterMovement = controller.GetControlledPlayerCharacter()->GetPlayerCharacterMovementComponent();
		characterMovement->SetWantsDive(true);

		return UndefinedState;
	}

	MouseInputStateTypeId MouseButtonElytraDiveState::OnMeleeButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (!mouseState.MeleeButton.Pressed) {
			OnRelease(controller);
			return GetId<MouseButtonEntryState>();
		}

		return UndefinedState;
	}

	MouseInputStateTypeId MouseButtonElytraDiveState::OnRootButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (!mouseState.RootButton.Pressed) {
			OnRelease(controller);
			return GetId<MouseButtonEntryState>();
		}

		return UndefinedState;
	}

	MouseInputStateTypeId MouseButtonElytraDiveState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (!controller.IsCustomMovementTypeActive(ECustomMovementType::Diving)) {
			return GetId<MouseButtonEntryState>();
		}
		return UndefinedState;
	}

	void MouseButtonElytraDiveState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		UPlayerCharacterMovementComponent* characterMovement = controller.GetControlledPlayerCharacter()->GetPlayerCharacterMovementComponent();
		characterMovement->SetWantsDive(false);
	}

	void MouseButtonElytraDiveState::OnRelease(ABasePlayerController& controller)
	{
		UPlayerCharacterMovementComponent* characterMovement = controller.GetControlledPlayerCharacter()->GetPlayerCharacterMovementComponent();
		characterMovement->SetWantsDive(false);
	}
}