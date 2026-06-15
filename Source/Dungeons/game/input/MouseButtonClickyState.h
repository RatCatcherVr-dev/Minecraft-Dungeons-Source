#pragma once
#include "MouseInputState.h"
#include <WeakObjectPtrTemplates.h>

class UInteractableComponent;

namespace input {

class MouseButtonClickyState : public IMouseInputState {
	TWeakObjectPtr<UInteractableComponent> clickyTarget;

	MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;
	MouseInputStateTypeId OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
	MouseInputStateTypeId OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
	MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

	void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

	MouseInputStateTypeId OnMouseButtonChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);

	EMouseCursorStates GetCursorState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;
	MouseInputStateTypeId GetInterruptState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;

private:
	bool UseClicky(ABasePlayerController& controller, const SButtonState&, unsigned int& lastClickedButtonVersion, const SMouseState& mouseState);
	unsigned int _lastClickedButtonVersion = 0; 
	unsigned int _lastClickedButtonVersionGamepad = 0; 
	float _pickupStamp = 0; 
};

}