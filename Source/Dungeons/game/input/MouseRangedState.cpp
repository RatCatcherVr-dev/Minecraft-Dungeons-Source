#include "Dungeons.h"
#include "MouseRangedState.h"
#include "MouseButtonEntryState.h"
#include "game/component/PlayerCharacterMovementComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "game/component/RangedAttackComponent.h"
#include "../component/AutoAimRangedAttackComponent.h"

namespace input {

	EMouseCursorStates MouseRangedState::GetCursorState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
	{
		if(mouseState.RangedButton.Pressed){
			return EMouseCursorStates::AttackHeld;
		}
		return EMouseCursorStates::UNSET;
	}

	MouseInputStateTypeId MouseRangedState::GetInterruptState(ABasePlayerController & controller, TargetController & targetController, const SMouseState & mouseState) const
	{
		if(!Intent){
			return GetId<MouseButtonEntryState>();
		}
		return UndefinedState;
	}

	bool MouseRangedState::CanEnterState(ABasePlayerController& controller)
	{
		if (const auto rangedComponent = controller.GetRangedAttackComponent()) {
			check(rangedComponent && "Must have ranged attack component to perform ranged attack!");
			//Don't allow entering state if we are out of ammo
			const auto character = controller.GetControlledPlayerCharacter();
			if (rangedComponent->HasAmmo() && rangedComponent->HasRangedWeaponEquipped() && character && !character->GetPlayerCharacterMovementComponent()->IsMovingWithElytra()) {
				return true;
			}			
		}
		
		return false;
	}

	void MouseRangedState::OnFailedToEnterState(ABasePlayerController& controller){
		if (const auto rangedComponent = controller.GetRangedAttackComponent()) {
			if (!rangedComponent->HasRangedWeaponEquipped()) {
				rangedComponent->OnNoRangedWeaponEquipped();
			}else if (!rangedComponent->HasAmmo()) {
				rangedComponent->OnInsuffientArrows();
			}
		}
	}

	input::MouseInputStateTypeId MouseRangedState::TryEnterState(ABasePlayerController& controller, const SMouseState& mouseState)
	{
		if (CanEnterState(controller)){
			return GetId<MouseRangedState>();
		} else {
			OnFailedToEnterState(controller);
		}		
		return UndefinedState;
	}

	MouseInputStateTypeId MouseRangedState::Tick(float deltaTime, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		auto rangedComponent = controller.GetRangedAttackComponent();
		if (!rangedComponent)
		{
			return UndefinedState;
		}

		auto world = controller.GetWorld();
		check(world && "Must have world to tick ranged state.");
		auto now = world->GetTimeSeconds();
		auto player = controller.GetControlledPlayerCharacter();
		check(player && "Must have controller in this state!");		
		auto canActSecond = player->GetCanActFromSecondActive();
		float canActFromSecondsPassive = player->GetCanActFromSecondPassive();
		auto smallestCooldown = player->GetCanActSmallestCooldown();
		auto timeAfterAttackToIgnoreTargeting = smallestCooldown * 0.5;
		
		/*
		 * Process intent here can accomplish two things:
		 *  1) End an ongoing attack if intent is no longer true
		 *	2) Begin a new attack if we are not attacking.
		 *
		 *	For 1) we are safe in that any ACTIVE attempt to start an attack will be handled either in OnEnterState
		 *	or in OnRangedButtonStateChanged.
		 *
		 *	For 2) We are guaranteed letting go of the mouse button will end the attack if we have not yet reached our
		 *	desired amount of attacks.
		 *
		 *	Thus we can safely not process the intent here when we are on passive cooldown, as we would only ever end up
		 *	needing to begin/end an attack here for passive reasons (i.e player holding down action button).
		*/
		if(now >= canActFromSecondsPassive) {
			ProcessIntentToBeginAttack(controller, targetController, mouseState);
		}

		if((canActSecond - timeAfterAttackToIgnoreTargeting) <= now){
			if (Intent && rangedComponent->IsAttacking() && StartedWithTarget && !targetController.GetAttackTarget().IsValid()) {
				if (rangedComponent->IsAutoAimEnabled()) {
					FVector loc;
					TWeakObjectPtr<AActor> AutoAimTarget;
					AutoAimTarget = rangedComponent->GetAutoAimTarget(loc);
					if (AutoAimTarget.IsValid() && isTargetable(*AutoAimTarget.Get())) {
						targetController.ForceSetTarget(AutoAimTarget.Get());
					}
				}
			}
			if ( Intent || ( DesiredAttackCounter > rangedComponent->IssuedAttackCounter && targetController.GetAttackTarget().IsValid() ) ){
				UpdateActorRotation(controller, targetController, mouseState);
			}
		}

		if (canActSecond <= now) {
			if ( ! Intent && DesiredAttackCounter <= rangedComponent->IssuedAttackCounter ) {
				if (mouseState.MoveButton.Pressed) return GetId<MouseMoveState>();
				if (mouseState.MoveAxis.Active) return GetId<MouseMoveState>();

				return GetId<MouseButtonEntryState>();
			}
		}

		return UndefinedState;
	}

	MouseInputStateTypeId MouseRangedState::OnMoveButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (mouseState.MoveButton.Pressed){
			//Abort and desired attacks when wanting to move.
			DesiredAttackCounter = 0;	
			Intent = false;
			if (const auto rangedComponent = controller.GetRangedAttackComponent()) {
				//Stop any current charged attack
				rangedComponent->Stop();
			}
		}
		return UndefinedState;
	}

	input::MouseInputStateTypeId MouseRangedState::OnMoveAxis(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState)
	{
		if (mouseState.MoveAxis.Active && !mouseState.RangedButton.Pressed)
		{
			DesiredAttackCounter = 0;
			Intent = false;
		}
		return UndefinedState;
		/* // D11.DB - This might need to be enabled on switch but it feels pretty bad
		   //			alongside the new analogue shooting controls.
		auto rangedComponent = controller.GetRangedAttackComponent();
		if (!rangedComponent)
		{
			return GetId<MouseMoveState>();
		}
		if (rangedComponent->HasAmmo())
		{
			return UndefinedState;
		}
		else
		{
			return GetId<MouseMoveState>();
		}
		*/
	}

	MouseInputStateTypeId MouseRangedState::OnRangedButtonStateChanged(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (mouseState.RangedButton.Pressed) {
			SetIntent(controller, targetController, true, mouseState);
		}
		else if (!mouseState.RangedButton.Pressed) {
			SetIntent(controller, targetController, false, mouseState);
		}

		return UndefinedState;
	}

	void MouseRangedState::ProcessIntentToBeginAttack(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		if (Intent) {
			BeginAttackWithIntent(controller, targetController, mouseState);
		} else {
			BeginAttackWithoutIntent(controller, targetController, mouseState);
		}
	}

	void MouseRangedState::BeginAttackWithIntent(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		const auto rangedComponent = controller.GetRangedAttackComponent();
		if (!rangedComponent)
			return;

		if (rangedComponent->HasAmmo()) {
			if (!rangedComponent->IsAttacking()) {
				rangedComponent->BeginAttack(FAttackAimProvider::CreateRaw(this, &MouseRangedState::GenerateTargetData, &controller, &targetController));
			}
		}
		else {
			rangedComponent->OnInsuffientArrows();
			Intent = false;
		}
	}

	void MouseRangedState::BeginAttackWithoutIntent(ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		const auto rangedComponent = controller.GetRangedAttackComponent();
		if (!rangedComponent) {
			return;
		}

		if (DesiredAttackCounter > rangedComponent->IssuedAttackCounter) {
			//We no longer have any intent to charge/attack, but we have not yet satisfied our desired attack.
			if (rangedComponent->EndAttackIsAttackExecution()) {
				//We need to begin attack and then end attack to actually launch an attack
				if (rangedComponent->IsAttacking()) {
					//wait until an execution would be successful.
					if (rangedComponent->CanAttack()) {
						auto mob = controller.GetAutoTargetSystem()->GetTarget();
						auto player = controller.GetControlledPlayerCharacter();

						//Keep launching attacks until condition is met.
						if (player && mob.IsValid() && mouseState.RangedButton.Gamepad)
						{
							player->RotatePlayerTowardsActor(mob.Get());
							rangedComponent->EndAttack(mob.Get());
						}
						else
						{
							auto target = mob.IsValid() ? mob.Get() : targetController.GetAttackTarget().Get();
							rangedComponent->EndAttack(target);
						}
					}
				} else {
					//Keep starting new attacks
					rangedComponent->BeginAttack(FAttackAimProvider::CreateRaw(this, &MouseRangedState::GenerateTargetData, &controller, &targetController));	
				}
			} else {
				if (!rangedComponent->IsAttacking()) {
					//Keep attacking until our condition is met.
					rangedComponent->BeginAttack(FAttackAimProvider::CreateRaw(this, &MouseRangedState::GenerateTargetData, &controller, &targetController));
				}
			}
		} else {
			if(rangedComponent->IsAttacking()){
				auto mob = controller.GetAutoTargetSystem()->GetTarget();
				auto target = mob.IsValid() ? mob.Get() : targetController.GetAttackTarget().Get();

				rangedComponent->EndAttack(target);
			}
			DesiredAttackCounter = 0;
		}
	}

	void MouseRangedState::SetIntent(ABasePlayerController& controller, TargetController& targetController, bool intent, const SMouseState& mouseState) {
		auto rangedComponent = controller.GetRangedAttackComponent();
		if (rangedComponent)
		{
			Intent = intent;
			if (Intent) {
				//When intent to attack has been set once, remember that we want to attack at least once.
				//This counter counts up when RangedComponent actually results in an attack.
				//Charge-release, tap-release, non-charged-weapon-autofire, ...
				DesiredAttackCounter = rangedComponent->IssuedAttackCounter + 1;
			}
			ProcessIntentToBeginAttack(controller, targetController, mouseState);
		}
	}

	MouseInputStateTypeId MouseRangedState::OnEnterState(ABasePlayerController& controller, TargetController& targetController, const IMouseInputState& previousState, const SMouseState& mouseState)
	{
		const auto rangedComponent = controller.GetRangedAttackComponent();
		check(rangedComponent && "Must have ranged attack component to perform ranged attack!");

		if (!rangedComponent)
			return UndefinedState;

		targetController.SetTargetType(TargetController::ETargetType::TargetOnly);
		targetController.SetTargetMode(TargetController::ETargetMode::OnlyNew);

		controller.StopMovement();

		//We started the state with a hovered target.
		StartedWithTarget = targetController.GetAttackTarget().IsValid();
		//Update aiming
		UpdateActorRotation(controller, targetController, mouseState);

		SetIntent(controller, targetController, true, mouseState);

		return UndefinedState;
	}

	void MouseRangedState::OnExitState(const IMouseInputState& nextState, ABasePlayerController& controller, TargetController& targetController, const SMouseState& mouseState) {
		auto rangedComponent = controller.GetRangedAttackComponent();
		check(rangedComponent && "must have ranged attack component");

		if (!rangedComponent)
			return;

		if (rangedComponent->IsAttacking()) {
			rangedComponent->EndAttack(targetController.GetAttackTarget().Get());
		}

		targetController.SetTargetType(TargetController::ETargetType::All);
		targetController.SetTargetMode(TargetController::ETargetMode::Default);
	}

	
	FAttackComponentAimData MouseRangedState::GenerateTargetData(ABasePlayerController* controller, TargetController* targetController) const {
		auto player = controller->GetControlledPlayerCharacter();		
		if(player){

			auto mob = controller->GetAutoTargetSystem()->GetTarget();
			auto target = mob.IsValid() ? mob.Get() : targetController->GetAttackTarget().Get();
			FAttackComponentAimData attackComponent;
			attackComponent.target = target;
			return attackComponent;
		}
		return FAttackComponentAimData();
	}

	void MouseRangedState::UpdateActorRotation(ABasePlayerController& controller, TargetController& targetController, const SMouseState mouseState) {
		auto player = controller.GetControlledPlayerCharacter();
		if (AActor* attackTarget = targetController.GetAttackTarget().Get()) {
			//Rotate towards it
			player->RotatePlayerTowardsActor(attackTarget);
		}
		else if( !mouseState.RangedButton.Gamepad ) {
			//Otherwise - just rotate towards the mouse				
			FHitResult hitResult;
			if (controller.GetHitResultUnderCursor((ECollisionChannel)ECustomTraceChannels::ArrowPlane, false, hitResult)) {
				player->RotatePlayerTowardsLocation(hitResult.Location);
			}
		}
		else {
			// D11.DB - Gamepad controls. 
			if (const auto rangedComponent = controller.GetRangedAttackComponent()) {
				
				auto autoTarget = controller.GetAutoTargetSystem();
				TWeakObjectPtr<AMobCharacter> mob;

				if (autoTarget->IsValidLowLevel())
				{
					mob = autoTarget->GetTarget();
				}

				if (mob.IsValid() && !rangedComponent->ShouldUseChargeUpAttack()) {
					// We only enter this block if we have a VALID AUTOTARGET and are using a CROSSBOW.

					float nextAttackTime = rangedComponent->CanAttackFromTimestampSeconds();
					float currentTime = controller.GetWorld()->GetTimeSeconds();

					if (mouseState.MoveAxis.Active && (nextAttackTime - currentTime) > 0.1f) {
						// If we're not about to fire we need to allow the player to control their character's rotation.
						auto destination = controller.ProjectInputAxesFromPlayer(FVector(mouseState.MoveAxis.X, mouseState.MoveAxis.Y, 0.0f));
						player->RotatePlayerTowardsLocation(destination);
					}
					else {
						// If we're about to fire then we need to take over the player's rotation to aim at the autotarget.
						player->RotatePlayerTowardsActor(mob.Get());
					}
				}
				else {
					// These conditions are default behaviour for ranged weapons.

					if (mouseState.MoveAxis.Active) {
						// If the player is using the left stick, rotate them towards the direction that they are pushing.

						auto destination = controller.ProjectInputAxesFromPlayer(FVector(mouseState.MoveAxis.X, mouseState.MoveAxis.Y, 0.0f));
						player->RotatePlayerTowardsLocation(destination);
					}
					else {
						// Keep the player facing forward if they're not touching the stick.

						auto destination = player->GetActorLocation() + (player->GetActorRotation().Quaternion().GetForwardVector());
						player->RotatePlayerTowardsLocation(destination);
					}
				}
			}
		}
	}
}