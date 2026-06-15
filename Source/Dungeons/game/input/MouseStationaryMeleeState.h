#pragma once
#include "MouseInputState.h"
#include <WeakObjectPtrTemplates.h>
#include <GameFramework/Actor.h>
#include "game/actor/character/mob/MobCharacter.h"

namespace input {

	class MouseStationaryMeleeState : public IMouseInputState {

		void OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		MouseInputStateTypeId OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		MouseInputStateTypeId OnMeleeButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		MouseInputStateTypeId Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) override;

		MouseInputStateTypeId OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) override;

		bool AttemptAttack(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState);

		EMouseCursorStates GetCursorState(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) const override;

		TWeakObjectPtr<AMobCharacter> MeleePriorityMob;
		TWeakObjectPtr<AMobCharacter> RotatingTargetMob;
		int LastMeleePressVersion = -1;

		bool AttackButtonHeld; // D11.DB
	};
}
