#pragma once
#include "MouseInputState.h"
#include <WeakObjectPtrTemplates.h>
#include <GameFramework/Actor.h>

namespace input {

	class MouseMeleeState : public IMouseInputState {
		MouseInputStateTypeId OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;
		void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		MouseInputStateTypeId OnMouseButtonChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);
		
		MouseInputStateTypeId GetInterruptState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;

	private:
		TWeakObjectPtr<AActor> AttackTarget;
		bool HeldIntent = false;
	};

	bool isTargetable(const AActor&);

}
