#include "Dungeons.h"
#include "MouseStationaryMeleeState.h"
#include "game/component/movement/MovementFlyingCommon.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "TargetController.h"

extern TAutoConsoleVariable<int32> CVarPriorityTarget;
extern TAutoConsoleVariable<int32> CVarMeleeAutoWalk;

namespace input {

	bool MouseStationaryMeleeState::AttemptAttack(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		auto meleeAttackComponent = controller.GetMeleeAttackComponent();

		bool AutoWalkEnabled = CVarMeleeAutoWalk.GetValueOnGameThread() >= 1;
		bool PriorityTargetEnabled = CVarPriorityTarget.GetValueOnGameThread() >= 1;
		if (!meleeAttackComponent)
			return false;

		if (meleeAttackComponent->CanAttack()) {
			if (mouseState.MeleeButton.Gamepad && mouseState.MeleeButton.Pressed) {
				// D11.RR - Prevents a crash that happened when the player held the attack button while mission ended.
				if (controller.GetAutoTargetSystem() == nullptr)
				{
					return false;
				}

				auto& mobs = controller.GetAutoTargetSystem()->GetTargets();
				auto player = controller.GetControlledPlayerCharacter();

				// D11.DB - Update this to point at the mob we want to attack.
				AMobCharacter* mob = nullptr;

				// D11.DB - Try the priority mob first.
				//			The priority mob is the last mob that was attacked that is still in attack range while
				//			the attack button is being held down. It takes priority over the autotarget system.
				if (MeleePriorityMob.IsValid() && MeleePriorityMob.Get()->IsAlive()) {
					if (meleeAttackComponent->IsWithinAttackRange(MeleePriorityMob.Get())) {
						mob = MeleePriorityMob.Get();
					}
				}
				MeleePriorityMob.Reset();

				// D11.DB - No priority mob, find the best candidate from the autotarget system.
				if (!mob) {
					for (auto targetOrder : mobs) {
						auto iMob = targetOrder.Mob;
						// D11.DB - We only really care about mobs that are in attack range here.
						if( iMob.IsValid() && meleeAttackComponent->IsWithinAttackRange(iMob.Get()) ) {
							mob = iMob.Get();
							break;
						}
					}
				}

				// D11.DB - We still don't have a mob. There is a special case here where if the player has
				//			been holding down the attack button along with the left stick then they will
				//			automatically move toward the closest out-of-range mob before attacking.
				if (AutoWalkEnabled && !mob && AttackButtonHeld && mouseState.MoveAxis.Active) {
					for (auto targetOrder : mobs) {
						auto iMob = targetOrder.Mob;
						if( iMob.IsValid() ) {
							controller.MoveToActor(iMob.Get());
							return false;
						}
					}
				}

				// D11.DB - If we have a target, tell the autotargeting system which mob we're targeting and rotate the player
				//			toward that target.
				if (mob) 
				{
					controller.GetAutoTargetSystem()->ForceTarget(mob);
					player->RotatePlayerTowardsAttackTarget(mob);

					if (player->IsRotatedTowardsAttackTarget(mob) || meleeAttackComponent->IsTargetInAttackWedge(mob))
					{
						// D11.DB - Valid mob, in range and the player is facing them. Attack it.
						meleeAttackComponent->AttackLocal(mob);
						if (PriorityTargetEnabled) 
						{
							MeleePriorityMob = mob;
						}
						RotatingTargetMob.Reset();
					}
					else
					{
						//this is our target
						RotatingTargetMob = mob;
					}
				}
				else
				{
					// D11.DB - Invalid mob - Rotate the player in the direction they're trying to turn and attack thin air.
					if (mouseState.MoveAxis.Active)
					{
						auto destination = controller.ProjectInputAxesFromPlayer(FVector(mouseState.MoveAxis.X, mouseState.MoveAxis.Y, 0.0f));
						player->RotatePlayerTowardsLocation(destination);
					}
					meleeAttackComponent->AttackLocal();
				}

				// D11.DB - Clear any applied range boost.
				meleeAttackComponent->SetRangeBoost();

				return true;
			}
			else {
				// D11.DB
				meleeAttackComponent->SetRangeBoost();

				auto potentialTarget = targetController.GetAttackTarget();
				if (potentialTarget.IsValid() && meleeAttackComponent->CanAttack(potentialTarget.Get())) {

					auto player = controller.GetControlledPlayerCharacter();
					auto attackTarget = potentialTarget.Get();
					player->RotatePlayerTowardsAttackTarget(attackTarget);

					if (player->IsRotatedTowardsAttackTarget(attackTarget) || meleeAttackComponent->IsTargetInAttackWedge(attackTarget)) {
						meleeAttackComponent->AttackLocal(attackTarget);
					}
					else {
						return false;
					}
				}
				else {
					controller.RotatePlayerTowardsCursor();
					meleeAttackComponent->AttackLocal();
				}
				return true;
			}
		}

		if (MeleePriorityMob.IsValid())
		{
			controller.GetAutoTargetSystem()->ForceTarget(MeleePriorityMob.Get());
		}

		return false;
	}

	MouseInputStateTypeId MouseStationaryMeleeState::OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		const auto now = controller.GetWorld()->GetTimeSeconds();
		const auto player = controller.GetControlledPlayerCharacter();

		if (mouseState.MoveButton.Pressed) {
			if (auto meleeAttackComponent = controller.GetMeleeAttackComponent()) {
				meleeAttackComponent->SetHoldingAttack(false);
			}
		}

		if (player && (!mouseState.MoveButton.Pressed && player->GetCanActFromSecondActive() <= now) || player->GetCanActFromSecondPassive() <= now) {
			return GetId<MouseButtonEntryState>();
		}

		return UndefinedState;
	}

	MouseInputStateTypeId MouseStationaryMeleeState::OnMeleeButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (!mouseState.MeleeButton.Pressed) {
			if (auto meleeAttackComponent = controller.GetMeleeAttackComponent()) {
				meleeAttackComponent->SetHoldingAttack(false);
			}
		}
		return UndefinedState;
	}

	MouseInputStateTypeId MouseStationaryMeleeState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState) {
		controller.StopMovement();
		targetController.SetTargetType(TargetController::ETargetType::TargetOnly);

		AttackButtonHeld = LastMeleePressVersion == mouseState.MeleeButton.PressVersion;

		// D11.DB - Determine whether or not the player gets a boost to their attack range.
		if (auto meleeAttackComponent = controller.GetMeleeAttackComponent()) {
			meleeAttackComponent->SetRangeBoost();
			meleeAttackComponent->SetHoldingAttack(true);
			if (auto autoTarget = controller.GetAutoTargetSystem()) {
				bool rangeBoost = !mouseState.RangedButton.Pressed && mouseState.MeleeButton.Gamepad && mouseState.MoveAxis.Active;
				if (rangeBoost && LastMeleePressVersion != mouseState.MeleeButton.PressVersion) {
					meleeAttackComponent->SetRangeBoost(autoTarget->GetRangeBoost());
					LastMeleePressVersion = mouseState.MeleeButton.PressVersion;
				}
			}
		}

		AttemptAttack(controller, targetController, mouseState);

		return UndefinedState;
	}

	EMouseCursorStates MouseStationaryMeleeState::GetCursorState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
	{
		return EMouseCursorStates::AttackHeld;
	}

	MouseInputStateTypeId MouseStationaryMeleeState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (controller.IsCustomMovementTypeActive(ECustomMovementType::Gliding)) {
			return GetId<MouseButtonEntryState>();
		}

        auto now = controller.GetWorld()->GetTimeSeconds();
        auto player = controller.GetControlledPlayerCharacter();

        // D11.DB - Clear the priority target if the player has decided to stop attacking.
        if (mouseState.MeleeButton.Gamepad && !mouseState.MeleeButton.Pressed) 
		{ 
			MeleePriorityMob.Reset();

			//we need to finish rotating and attacking
			if (RotatingTargetMob.IsValid())
			{
				if (RotatingTargetMob.Get()->IsAlive())
				{
					controller.GetAutoTargetSystem()->ForceTarget(RotatingTargetMob.Get());
					auto meleeAttackComponent = controller.GetMeleeAttackComponent();
					if (meleeAttackComponent->IsWithinAttackRange(RotatingTargetMob.Get()))
					{
						if (player->IsRotatedTowardsActor(RotatingTargetMob.Get()) || meleeAttackComponent->IsTargetInAttackWedge(RotatingTargetMob.Get()))
						{
							// D11.SC - Valid mob, in range and the player is facing them. Attack it.
							meleeAttackComponent->AttackLocal(RotatingTargetMob.Get());
							RotatingTargetMob.Reset();
						}

						return UndefinedState;
					}
					else
					{
						RotatingTargetMob.Reset();
					}
				}
				else
				{
					RotatingTargetMob.Reset();
				}
			}
        }

        if( !(mouseState.MeleeButton.Gamepad && mouseState.MeleeButton.Pressed )) {
            if (((!mouseState.RootButton.Pressed && player->GetCanActFromSecondActive() <= now) ||
                (!mouseState.MoveButton.Pressed && player->GetCanActFromSecondActive() <= now))) {
                return GetId<MouseButtonEntryState>();
            }
        }

        //Try to attack.
        if (!AttemptAttack(controller, targetController, mouseState)) {
            //If we failed to attack...
            auto meleeAttackComponent = controller.GetMeleeAttackComponent();

            //We've passed the actionable point and should react to input again
            if (meleeAttackComponent && player && player->GetCanActFromSecondPassive() <= now) {
                auto potentialTarget = targetController.GetAttackTarget();
                //If we have a potential target in range, we should rotate towards that, otherwise look towards the cursor.
                if (potentialTarget.IsValid() && meleeAttackComponent->IsWithinAttackRange(potentialTarget.Get())) {
                    player->RotatePlayerTowardsAttackTarget(potentialTarget.Get());
                }
                else {
                    if( mouseState.MeleeButton.Gamepad ) {
                        if (mouseState.MoveAxis.Active) {
                            auto destination = controller.ProjectInputAxesFromPlayer(FVector(mouseState.MoveAxis.X, mouseState.MoveAxis.Y, 0.0f));
                            player->RotatePlayerTowardsLocation(destination);
                        }
                    }
                    else {
                        controller.RotatePlayerTowardsCursor();
                    }
                }
            }
        }

        return UndefinedState;
	}

	void MouseStationaryMeleeState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		auto meleeAttackComponent = controller.GetMeleeAttackComponent();
		if (meleeAttackComponent)
		{
			meleeAttackComponent->SetHoldingAttack(false);
			meleeAttackComponent->SetRangeBoost();
		}
		targetController.SetTargetType(TargetController::ETargetType::All);
	}

}
