#include "Dungeons.h"
#include "MouseButtonEntryState.h"
#include "MouseMeleeState.h"
#include "MouseMoveState.h"
#include "MouseRangedState.h"
#include "TargetController.h"
#include "MouseButtonClickyState.h"
#include "MouseStationaryMeleeState.h"
#include "MouseContinuousMeleeState.h"
#include "MouseButtonElytraDiveState.h"
#include "game/component/movement/MovementFlyingCommon.h"
#include "game/component/AutoAimRangedAttackComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "MouseDynamicRootState.h"

namespace input {

	MouseInputStateTypeId MouseButtonEntryState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) {
		return Transition(controller, targetController, mouseState);
	}
	
	MouseInputStateTypeId MouseButtonEntryState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		return Transition(controller, targetController, mouseState);
	}
	
	MouseInputStateTypeId MouseButtonEntryState::Transition(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if(controller.IsCustomMovementTypeActive(ECustomMovementType::BlastingOff))
		{
			return mouseState.MoveAxis.Active ? GetId<MouseMoveState>() : UndefinedState;
		}

		if (mouseState.RangedButton.Pressed) {
			if (MouseRangedState::CanEnterState(controller)) {
				return GetId<MouseRangedState>();
			} else if (!mouseState.RangedButton.IsBeingHeld()){
				MouseRangedState::OnFailedToEnterState(controller);
			}
		}
		
		if(mouseState.RootButton.Pressed && controller.IsCustomMovementTypeActive(ECustomMovementType::Gliding)){
			return GetId<MouseButtonElytraDiveState>();
		}
		else if (mouseState.MoveButton.Pressed) {
			if (mouseState.RootButton.Pressed) {
				return GetId<MouseStationaryMeleeState>();
			}
			else if (targetController.GetAttackTarget().IsValid()) {
				return GetId<MouseContinuousMeleeState>();
			}
			else if (targetController.GetClickyTarget().IsValid()) {
				return GetId<MouseButtonClickyState>();
			}
			else if (controller.GetControlledPlayerCharacter()->IsFrozenSolid()){
				return GetId<MouseDynamicRootState>();
			}
			else {
				return GetId<MouseMoveState>();
			}
		}
		else if (mouseState.MeleeButton.Pressed) { // D11.DB
			if (controller.IsCustomMovementTypeActive(ECustomMovementType::Gliding)) {
				return GetId<MouseButtonElytraDiveState>();
			}
			else if (targetController.GetClickyTarget().IsValid()) {
				mLastMeleeId = mouseState.MeleeButton.PressVersion;
				return GetId<MouseButtonClickyState>();
			}
			else if( mLastMeleeId != mouseState.MeleeButton.PressVersion ){
				return GetId<MouseStationaryMeleeState>();
			}
		}
		else if (mouseState.MoveAxis.Active) { // D11.DB
			if (mouseState.RootButton.Pressed && mouseState.RootButton.Gamepad) {
				return GetId<MouseDynamicRootState>();
			}
			else if (controller.GetControlledPlayerCharacter()->IsFrozenSolid()) {
				return GetId<MouseDynamicRootState>();
			}
			else {
				return GetId<MouseMoveState>();
			}
		}
	
		return UndefinedState;
	}

	EMouseCursorStates MouseButtonEntryState::GetCursorState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
	{
		if (mouseState.RootButton.Pressed) {
			return EMouseCursorStates::Attack;
		} 
		else if (targetController.GetAttackTarget().IsValid()) {
			return EMouseCursorStates::Attack;
		}
		else if (targetController.GetClickyTarget().IsValid()) {
			return EMouseCursorStates::Interact;
		}
		return EMouseCursorStates::Move;
	}

}