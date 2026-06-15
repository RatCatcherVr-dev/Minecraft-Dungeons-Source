#pragma once
#include "MouseInputState.h"
#include <WeakObjectPtrTemplates.h>
#include "game/component/PlayerCharacterMovementComponent.h"

namespace input {

class MouseButtonElytraDiveState : public IMouseInputState {
	MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;
	MouseInputStateTypeId OnMeleeButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
	MouseInputStateTypeId OnRootButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
	MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
	void OnRelease(ABasePlayerController& controller);
	void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
};

}
