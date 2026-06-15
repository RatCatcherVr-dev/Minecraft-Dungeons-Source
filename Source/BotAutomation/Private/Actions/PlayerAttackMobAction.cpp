#include "BotAutomationPCH.h"
#include "PlayerAttackMobAction.h"

#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"

#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/actor/character/player/BasePlayerController.h"

#include "game/component/MeleeAttackComponent.h"
#include "game/component/AutoAimRangedAttackComponent.h"

#include "game/util/ActorQuery.h"
#include "game/team/TeamQuery.h"

void PlayerAttackMobAction::StartAction()
{
	Super::StartAction();

	if (auto PlayerController = GetBasePlayerController()) {
		SetTarget(PlayerController, CurrentTarget);
		if (CurrentTarget.IsValid()) {
			PlayerAutomator->MoveToActor(CurrentTarget.Get());
		}
	}
}

void PlayerAttackMobAction::StopAction()
{
	if (bHasMeleePressed || bHasRangedPressed) {
		if (auto PlayerController = GetBasePlayerController()) {
			ReleasePlayerAttackButton(PlayerController);
		}
	}

	Super::StopAction();
}

bool PlayerAttackMobAction::ExecuteAction(float DeltaSeconds)
{
	if (const auto PlayerController = GetBasePlayerController())
	{
		if (bHasMeleePressed || bHasRangedPressed) {
			ReleasePlayerAttackButton(PlayerController);
			return true;
		}

		const auto playerCharacter = GetPlayerCharacter(PlayerController);

		if (!IsValidTarget(playerCharacter, CurrentTarget)) {
			if (bHasAttacked) {
				EndAction(EPlayerBotActionResult::Success);
			}
			else {
				EndAction(EPlayerBotActionResult::Error);
			}
			return false;
		}

		const auto meleeComponent = PlayerController->GetMeleeAttackComponent();

		if (bAllowRanged)
		{
			const auto isMeleeAttacking = meleeComponent ? meleeComponent->IsWithinAttackRange(CurrentTarget.Get()) || meleeComponent->IsAttackInProgress() : false;
			if (!isMeleeAttacking)
			{
				const auto rangedComponent = PlayerController->GetRangedAttackComponent();
				if (rangedComponent) RangedAttack(PlayerController, playerCharacter, rangedComponent, CurrentTarget.Get());
			}
		}

		if (meleeComponent) MeleeAttack(PlayerController, playerCharacter, meleeComponent, CurrentTarget.Get());
	}

	return true;
}

void PlayerAttackMobAction::MeleeAttack(ABasePlayerController* PlayerController, APlayerCharacter* Character, UMeleeAttackComponent* MeleeComponent, ABaseCharacter* Target)
{
	if (MeleeComponent->IsWithinAttackRange(Target))
	{
		Character->RotatePlayerTowardsActor(Target);

		if (Character->IsRotatedTowardsActor(Target) || MeleeComponent->IsTargetInAttackWedge(Target))
		{
			PlayerAutomator->StopMovement();

			if (!MeleeComponent->IsAttacking() && !MeleeComponent->IsAttackInProgress())
			{
				PlayerController->OnMeleeAttackButtonPressed();
				bHasMeleePressed = true;
				bHasAttacked = true;
			}
		}
	}
	else
	{
		PlayerAutomator->MoveToActor(Target);
	}
}

void PlayerAttackMobAction::RangedAttack(ABasePlayerController* PlayerController, APlayerCharacter* Character, UAutoAimRangedAttackComponent* RangedComponent, ABaseCharacter* Target, float Range)
{
	if (!(RangedComponent->HasAmmo() && RangedComponent->HasRangedWeaponEquipped())) {
		return;
	}

	const auto attackRange = Range > 0 ? Range : RangedComponent->GetAttackRange();
	const auto distance = actorquery::getCapsuleDistance2D(Character, Target);
	if (distance <= attackRange)
	{
		if (IsTargetInLOS(Character, Target))
		{
			Character->RotatePlayerTowardsActor(Target);

			if (Character->IsRotatedTowardsActor(Target)) {
				PlayerAutomator->StopMovement();
			}

			if (!RangedComponent->IsAttacking() && !RangedComponent->IsAttackInProgress())
			{
				PlayerController->OnRangedAttackButtonGamepadPressed();
				bHasRangedPressed = true;
				bHasAttacked = true;
			}
			else if (RangedComponent->IsAttackInProgress() && RangedComponent->ShouldUseChargeUpAttack()) {
				ReleasePlayerAttackButton(PlayerController);
			}
		}
	}
	else {
		PlayerAutomator->MoveToActor(Target);
	}
}

void PlayerAttackMobAction::DebugDraw()
{
	Super::DebugDraw();

#if !UE_BUILD_SHIPPING
	PlayerAutomator->DebugDrawPathFind();

	const auto playerCharacter = GetPlayerCharacter(GetBasePlayerController());

	if (IsValidTarget(playerCharacter, CurrentTarget))
	{
		const auto world = PlayerAutomator->GetWorld();
		const auto source = playerCharacter->GetActorLocation();
		const auto target = CurrentTarget->GetActorLocation();

		DrawDebugLine(world, source, target, FColor::Emerald, false, -1, 0, 2.5f);
	}
#endif
}

bool PlayerAttackMobAction::IsValidTarget(const APlayerCharacter* player, TWeakObjectPtr<ABaseCharacter> target)
{
	return target.IsValid() && IsTargetable(player, target);
}

bool PlayerAttackMobAction::IsTargetable(const APlayerCharacter* player, TWeakObjectPtr<ABaseCharacter> target) {
	if (target->IsPendingKillOrUnreachable())
	{
		return false;
	}

	if (!target->IsTargetable() || !target->IsAlive()) {
		return false;
	}

	return teamquery::can::damage(player->GetCurrentTeam(), target->GetCurrentTeam());
}

void PlayerAttackMobAction::SetTarget(ABasePlayerController* PlayerController, TWeakObjectPtr<ABaseCharacter> Target)
{
	ReleasePlayerAttackButton(PlayerController);

	if (Target == CurrentTarget) {
		return;
	}

	if (IsValidTarget(GetPlayerCharacter(PlayerController), Target))
	{
		CurrentTarget = Target;
		PlayerController->GetTargetController().ForceSetTarget(Target.Get());
		PlayerController->OnActorClicked(Target.Get(), EClickTargetType::Target);
	}
	else {
		CurrentTarget = nullptr;
	}
}

bool PlayerAttackMobAction::IsTargetInLOS(APlayerCharacter* Character, TWeakObjectPtr<ABaseCharacter> Target)
{
	if (!IsValidTarget(Character, Target)) {
		return false;
	}

	const auto world = Character->GetWorld();
	const auto source = Character->GetActorLocation();
	const auto target = Target->GetActorLocation();

	FHitResult hitResult;
	world->LineTraceSingleByChannel(hitResult, source, target, (ECollisionChannel)ECustomTraceChannels::IgnorePlayer);
	if (hitResult.bBlockingHit)
	{
		if (AActor* hitActor = hitResult.GetActor()) {
			if (hitActor == Target.Get()) {
				return true;
			}
			else {
				if (auto hitMob = Cast<AMobCharacter>(hitActor)) {
					return IsValidTarget(Character, hitMob);
				}
			}
		}
	}

	return false;
}

void PlayerAttackMobAction::ReleasePlayerAttackButton(ABasePlayerController* PlayerController)
{
	if (bHasMeleePressed) {
		PlayerController->OnMeleeAttackButtonReleased();
	}

	if (bHasRangedPressed) {
		PlayerController->OnRangedAttackButtonGamepadReleased();
	}

	if (bHasMeleePressed || bHasRangedPressed)
	{
		PlayerController->OnRootPlayerGamepadReleased();
		bHasMeleePressed = false;
		bHasRangedPressed = false;
	}
}
