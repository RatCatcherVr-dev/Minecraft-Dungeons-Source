#pragma once
#include "MouseInputState.h"
#include "game/component/AttackComponent.h"

namespace input {

	class MouseRangedState : public IMouseInputState {

	public:
		MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnMoveAxis(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;
		MouseInputStateTypeId OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;
		void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		FAttackComponentAimData GenerateTargetData(ABasePlayerController* controller, TargetController* targetController) const;

		EMouseCursorStates GetCursorState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;
		MouseInputStateTypeId GetInterruptState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;

		static bool CanEnterState(ABasePlayerController& controller);
		static void OnFailedToEnterState(ABasePlayerController& controller);
		static MouseInputStateTypeId TryEnterState(ABasePlayerController& controller, const SMouseState& mouseState);
	private:
		void ProcessIntentToBeginAttack(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);
		void BeginAttackWithIntent(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);
		void BeginAttackWithoutIntent(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);

		void UpdateActorRotation(ABasePlayerController& controller, TargetController& targetController, const SMouseState mouseState);
		void SetIntent(ABasePlayerController& controller, TargetController& targetController, bool intent, const SMouseState& mouseState);

		bool StartedWithTarget = false;		
		bool Intent = false;
		int DesiredAttackCounter = 0;		

	};

}
