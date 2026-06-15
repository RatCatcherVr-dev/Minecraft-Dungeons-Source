#include "Dungeons.h"
#include "MouseButtonClickyState.h"
#include "TargetController.h"
#include "MouseButtonEntryState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/InteractableComponent.h"
#include "../util/DungeonsEffectLibrary.h"

namespace input {

MouseInputStateTypeId MouseButtonClickyState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) {
	clickyTarget = targetController.GetClickyTarget();
	
	if (!clickyTarget.IsValid()) {
		return GetId<MouseButtonEntryState>();
	}
	auto clickyOwner = clickyTarget->GetOwner();
	
	targetController.SetTargetMode(TargetController::ETargetMode::Sticky);

	if (controller.GetGamepadActive()) {
		if (!UseClicky(controller, mouseState.MeleeButton, _lastClickedButtonVersionGamepad, mouseState)) {
			return GetId<MouseStationaryMeleeState>();
		}
		else
		{
			controller.OnActorClicked(clickyOwner, EClickTargetType::Clicky);
		}
	}
	else if (!UseClicky(controller, mouseState.MoveButton, _lastClickedButtonVersion, mouseState)) {
		const auto pointers = clickyTarget.Get()->GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(), "pointer");
		if (pointers.Num() > 0) {
			const auto location = Cast<USceneComponent>(pointers[0])->GetComponentLocation();
			controller.MoveToLocation(location);
		} else {
			controller.MoveToActor(clickyTarget.Get()->GetOwner());
		}
	}
	else
	{
		controller.OnActorClicked(clickyOwner, EClickTargetType::Clicky);
	}

	return UndefinedState;
}

void MouseButtonClickyState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
	targetController.SetTargetMode(TargetController::ETargetMode::Default);
}

MouseInputStateTypeId MouseButtonClickyState::OnMouseButtonChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
	if (!mouseState.MoveButton.Pressed) {
		targetController.SetTargetMode(TargetController::ETargetMode::Default);
		return UndefinedState;
	}
	return GetInterruptState(controller, targetController, mouseState);
}


MouseInputStateTypeId MouseButtonClickyState::OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {	
	return OnMouseButtonChanged(controller, targetController, mouseState);
}

MouseInputStateTypeId MouseButtonClickyState::OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
	return OnMouseButtonChanged(controller, targetController, mouseState);
}


MouseInputStateTypeId MouseButtonClickyState::GetInterruptState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
{
	return GetId<MouseButtonEntryState>();
}

EMouseCursorStates MouseButtonClickyState::GetCursorState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
{
	if (mouseState.RootButton.Pressed) {
		return EMouseCursorStates::Attack;
	}
	if (mouseState.MoveButton.Pressed) {
		return EMouseCursorStates::InteractHeld;
	}
	return EMouseCursorStates::UNSET;
}


MouseInputStateTypeId MouseButtonClickyState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
	if (!clickyTarget.IsValid() || UseClicky(controller, mouseState.MoveButton, _lastClickedButtonVersion, mouseState)) {
		if (controller.GetGamepadActive()) {
			if (!mouseState.MeleeButton.Pressed || UGameplayStatics::GetTimeSeconds(&controller) > _pickupStamp + MouseInputStateMachine::GetClickGraceTimeSeconds()) {
				return GetId<MouseButtonEntryState>();
			}
		}
		else {
			if (!mouseState.MoveButton.Pressed || UGameplayStatics::GetTimeSeconds(&controller) > _pickupStamp + MouseInputStateMachine::GetClickGraceTimeSeconds()) {
				return GetId<MouseButtonEntryState>();
			}
		}
	}

	if (mouseState.MoveButton.Pressed && mouseState.RootButton.Pressed) {
		return GetId<MouseStationaryMeleeState>();
	}

	if (mouseState.RangedButton.Pressed) {
		return GetId<MouseRangedState>();
	}

	if (controller.GetGamepadActive() && mouseState.MoveAxis.Active) {
		return GetId<MouseButtonEntryState>();
	}

	return UndefinedState;
}

bool MouseButtonClickyState::UseClicky(ABasePlayerController& controller, const SButtonState& buttonState, unsigned int& lastClickedButtonVersion, const SMouseState& mouseState) {
	if (buttonState.PressVersion == lastClickedButtonVersion) {
		// Still the same click as the last time the clicky was used (button being held)
		return false;
	}

	if (buttonState.Gamepad && buttonState.IsBeingHeld()) {
		// #D11.CM - Interactions need to be distinct presses, not held buttons.
		return false;
	}

	const auto actorLocation = controller.GetCharacter()->GetActorLocation();
	FVector closestClickyPoint;
	clickyTarget->GetOwner()->ActorGetDistanceToCollision(actorLocation, (ECollisionChannel)ECustomTraceChannels::IgnorePlayer, closestClickyPoint);
	const auto distanceSquared = FVector::DistSquared2D(actorLocation, closestClickyPoint);

	auto radius = clickyTarget->GetRadius();
	if (controller.GetGamepadActive()) {
		radius += clickyTarget->GetRadiusGamepadModifier();
	}

	if ((distanceSquared > FMath::Square(radius)) || !clickyTarget->IsInteractionEnalbed()) {
		return false;
	}

	if (clickyTarget->bOneInteractionPerPlayer && clickyTarget->HasBeenUsedBy(controller.GetControlledPlayerCharacter())) {
		return false;
	}

	clickyTarget->Interacted(controller.GetControlledPlayerCharacter());
	_pickupStamp = UGameplayStatics::GetTimeSeconds(&controller);
	lastClickedButtonVersion = buttonState.PressVersion;
	controller.StopMovement();
	return true;
}

}