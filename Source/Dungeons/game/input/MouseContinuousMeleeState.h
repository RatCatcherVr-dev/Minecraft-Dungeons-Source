#pragma once
#include "MouseInputState.h"
#include <WeakObjectPtrTemplates.h>
#include <GameFramework/Actor.h>

namespace input {

	class MouseContinuousMeleeState : public IMouseInputState {
	
		MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;
		void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		EMouseCursorStates GetCursorState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;

	private:
		float TargetReacquisitionTimer = 0.f;
		bool AttackOnEnd = false;
		bool HeldIntent = false;
		TWeakObjectPtr<AActor> LastAttackTarget;
	};
}