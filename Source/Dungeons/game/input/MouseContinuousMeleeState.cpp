#include "Dungeons.h"
#include "MouseContinuousMeleeState.h"
#include "MouseButtonEntryState.h"
#include "MouseMeleeState.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/util/DungeonsEffectLibrary.h"
#include <Particles/ParticleSystemComponent.h>


namespace input {

	EMouseCursorStates MouseContinuousMeleeState::GetCursorState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
	{
		return EMouseCursorStates::AttackHeld;
	}

	MouseInputStateTypeId MouseContinuousMeleeState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		auto now = controller.GetWorld()->GetTimeSeconds();
		auto player = controller.GetControlledPlayerCharacter();
		auto attackComponent = controller.GetMeleeAttackComponent();
		auto pathFollowingComponent = controller.GetPathFollowingComponent();
		auto AttackTarget = targetController.GetAttackTarget().Get();

		if (!attackComponent) 
			return UndefinedState;
	
		AttackOnEnd = AttackOnEnd && mouseState.MoveButton.TimeInState <= std::chrono::duration_cast<std::chrono::duration<float>>(MouseInputStateMachine::ClickGraceTime).count();
	
		//Always wait for exit when Held intent no longer is true. If not, we can get stuck in an infinite attack
		if (HeldIntent) {
			//Move and attack
			if (AttackTarget) {
				if (LastAttackTarget.Get() != AttackTarget) {
					controller.OnActorClicked(AttackTarget, EClickTargetType::Target);
					LastAttackTarget = AttackTarget;
				}
	
				TargetReacquisitionTimer = controller.TargetReacquisitionTimeoutSeconds;
				bool isMoving = pathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle;
				if (attackComponent->CanAttack(AttackTarget)) {
					player->RotatePlayerTowardsAttackTarget(AttackTarget);
					controller.StopMovement();
	
					if (player->IsRotatedTowardsAttackTarget(AttackTarget) || attackComponent->IsTargetInAttackWedge(AttackTarget)) {
						attackComponent->AttackLocal(AttackTarget);
						AttackOnEnd = false;
					}
				}
				else {
					bool canMove = player->GetCanActFromSecondPassive() <= now;
					bool needToMove = !attackComponent->IsWithinAttackRange(AttackTarget);
		
					if (needToMove && canMove && !isMoving) {
						controller.MoveToActor(AttackTarget);
					}
				}
			}
			else {
				TargetReacquisitionTimer -= deltaTime;
				//Move towards cursor
				bool canMove = player->GetCanActFromSecondPassive() <= now;
				if(canMove){
					bool isMoving = pathFollowingComponent->GetStatus() != EPathFollowingStatus::Idle;
					FVector target = pathFollowingComponent->GetCurrentTargetLocation();
					FHitResult hitResult;
					controller.GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType((ECollisionChannel)ECustomTraceChannels::PlayerPlane), false, hitResult);
					if(!isMoving || !target.Equals(hitResult.Location)){
						controller.MoveToLocation(hitResult.Location, -1.f, true, false);
					}
				}
			}
		}
	
	
		//Handle exiting
		if (!HeldIntent && player->GetCanActFromSecondActive() <= now) {
			return AttackOnEnd ? GetId<MouseMeleeState>() : GetId<MouseButtonEntryState>();
		}
		else if (!AttackTarget && TargetReacquisitionTimer <= 0) {
			return GetId<MouseMoveState>();
		}
	
		return UndefinedState;
	}
	

	MouseInputStateTypeId MouseContinuousMeleeState::OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (!mouseState.MoveButton.Pressed) {
			HeldIntent = false;
			if (auto meleeAttackComponent = controller.GetMeleeAttackComponent()) {
				meleeAttackComponent->SetHoldingAttack(false);
			}
		}

		return UndefinedState;
	}

	
	MouseInputStateTypeId MouseContinuousMeleeState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) {
		AttackOnEnd = true;
		HeldIntent = true;
		TargetReacquisitionTimer = controller.TargetReacquisitionTimeoutSeconds;
		auto AttackTarget = targetController.GetAttackTarget().Get();

		if (!AttackTarget) {
			return GetId<MouseButtonEntryState>();
		}

		LastAttackTarget = AttackTarget;
		controller.OnActorClicked(AttackTarget, EClickTargetType::Target);
	
		targetController.SetTargetType(TargetController::ETargetType::TargetOnly);
		targetController.SetTargetMode(TargetController::ETargetMode::StickyReplaceEmpty);

		//When initially requesting an attack, we should always move to the correct spot for attacking the target.
		bool atTarget = controller.MoveToActor(AttackTarget) == EPathFollowingRequestResult::AlreadyAtGoal;
	
		//If we're already at the required location, we immediately attack.
		auto attackComponent = controller.GetMeleeAttackComponent();
		attackComponent->SetHoldingAttack(true);
		if (attackComponent && attackComponent->CanAttack(AttackTarget) && atTarget) {

			auto Character = controller.GetControlledPlayerCharacter();
			Character->RotatePlayerTowardsAttackTarget(AttackTarget);

			if(Character->IsRotatedTowardsAttackTarget(AttackTarget) || attackComponent->IsTargetInAttackWedge(AttackTarget)){
				AttackOnEnd = false;
				attackComponent->AttackLocal(AttackTarget);
			}
		}
	
		return UndefinedState;
	}
	
	void MouseContinuousMeleeState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		targetController.SetTargetMode(TargetController::ETargetMode::Default);
		targetController.SetTargetType(TargetController::ETargetType::All);
		if (auto meleeAttackComponent = controller.GetMeleeAttackComponent()) {
			meleeAttackComponent->SetHoldingAttack(false);
		}
	}

}