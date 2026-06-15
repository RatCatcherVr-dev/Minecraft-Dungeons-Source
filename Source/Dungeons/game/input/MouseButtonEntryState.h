#pragma once
#include "MouseInputState.h"

namespace input {

	class MouseButtonEntryState : public IMouseInputState {
		MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;
	
		MouseInputStateTypeId Transition(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);

		EMouseCursorStates GetCursorState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;		

		unsigned int mLastMeleeId;
	};
}