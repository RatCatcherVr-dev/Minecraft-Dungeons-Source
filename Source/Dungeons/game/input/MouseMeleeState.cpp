#include "Dungeons.h"
#include "MouseMeleeState.h"

#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/MeleeAttackComponent.h"
#include "TargetController.h"
#include "game/actor/character/player/PlayerCharacter.h"


namespace input {

	MouseInputStateTypeId MouseMeleeState::OnMouseButtonChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (!mouseState.MoveButton.Pressed) {
			if (auto meleeAttackComponent = controller.GetMeleeAttackComponent()) {
				meleeAttackComponent->SetHoldingAttack(false);
			}
			HeldIntent = false;
		}

		return GetInterruptState(controller, targetController, mouseState);
	}

	MouseInputStateTypeId MouseMeleeState::GetInterruptState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
	{
		APlayerCharacter* player = controller.GetControlledPlayerCharacter();
		float now = controller.GetWorld()->GetTimeSeconds();
		if (player->GetCanActFromSecondActive() <= now) {
			return GetId<MouseButtonEntryState>();
		}

		return UndefinedState;
	}


	MouseInputStateTypeId MouseMeleeState::OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		return OnMouseButtonChanged(controller, targetController, mouseState);
	}

	MouseInputStateTypeId MouseMeleeState::OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		return OnMouseButtonChanged(controller, targetController, mouseState);
	}

	MouseInputStateTypeId MouseMeleeState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		APlayerCharacter* player = controller.GetControlledPlayerCharacter();
		float now = controller.GetWorld()->GetTimeSeconds();
		if (!AttackTarget.IsValid() || !isTargetable(*AttackTarget.Get())) {
			if ((HeldIntent ? player->GetCanActFromSecondPassive() : player->GetCanActFromSecondActive()) <= now) {
				return GetId<MouseButtonEntryState>();
			}
		}
		else if (AttackTarget.IsValid()) {

			auto ValidAttackTarget = AttackTarget.Get();
			auto meleeAttackComponent = controller.GetMeleeAttackComponent();

			if (meleeAttackComponent && meleeAttackComponent->CanAttack(ValidAttackTarget)) {

				controller.StopMovement();
				player->RotatePlayerTowardsAttackTarget(ValidAttackTarget);

				if (player->IsRotatedTowardsAttackTarget(ValidAttackTarget) || meleeAttackComponent->IsTargetInAttackWedge(ValidAttackTarget)) {
					meleeAttackComponent->AttackLocal(ValidAttackTarget);
					AttackTarget.Reset();
				}
			}
		}

		return UndefinedState;
	}


	MouseInputStateTypeId MouseMeleeState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) {
		AttackTarget = targetController.GetAttackTarget();

		HeldIntent = true;

		if (auto meleeAttackComponent = controller.GetMeleeAttackComponent()) {
			meleeAttackComponent->SetHoldingAttack(true);
		}

		if (!AttackTarget.IsValid()) {
			return GetId<MouseButtonEntryState>();
		}

		return UndefinedState;
	}

	void MouseMeleeState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (auto meleeAttackComponent = controller.GetMeleeAttackComponent()) {
			meleeAttackComponent->SetHoldingAttack(false);
		}
	}

	bool isTargetable(const AActor& actor) {
		const auto targetMob = Cast<AMobCharacter>(&actor);
		if (targetMob == nullptr) {
			return true;
		}
		else if (targetMob->IsTargetable()) {
			auto mobHitHealth = targetMob->FindComponentByClass<UHealthComponent>();
			if (mobHitHealth == nullptr || mobHitHealth->IsAlive()) {
				return true;
			}
		}
		return false;
	}

}