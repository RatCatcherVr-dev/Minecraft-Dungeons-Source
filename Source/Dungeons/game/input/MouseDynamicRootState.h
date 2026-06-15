#pragma once
#include "MouseInputState.h"

namespace input {

	class MouseDynamicRootState : public IMouseInputState {

	public:
		MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;
		void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnMoveAxis(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
	};

}
