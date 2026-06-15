#include "BotAutomationPCH.h"
#include "PlayerAttackRadiusAction.h"

#include "GameFramework/Actor.h"

#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"

#include "game/actor/character/player/PlayerControllerBase.h"
#include "game/actor/character/player/BasePlayerController.h"

#include "game/component/MeleeAttackComponent.h"
#include "game/component/AutoAimRangedAttackComponent.h"

#include "game/util/ActorQuery.h"

void PlayerAttackRadiusAction::StartAction()
{
	Super::StartAction();
	PlayerAutomator->MoveToLocation(Location);
}

void PlayerAttackRadiusAction::StopAction()
{
	Super::StopAction();
	SetStatus(EPlayerBotActionResult::Success);
}

bool PlayerAttackRadiusAction::ExecuteAction(float DeltaSeconds)
{
	if (auto PlayerController = GetBasePlayerController())
	{
		const auto playerCharacter = GetPlayerCharacter(PlayerController);

		if (ShouldTargetNewActor(playerCharacter, CurrentTarget, AttackRadius))
		{
			auto targetCharacter = GetClosestCharacter(PlayerController->GetCharacter(), AttackRadius, PlayerController->GetUniqueID());

			SetTarget(PlayerController, targetCharacter);
		}

		if (IsValidTarget(playerCharacter, CurrentTarget))
		{
			Location = CurrentTarget->GetActorLocation();

			const auto meleeComponent = PlayerController->GetMeleeAttackComponent();

			if (bAllowRanged)
			{
				const auto isMeleeAttacking = meleeComponent ? meleeComponent->IsWithinAttackRange(CurrentTarget.Get()) || meleeComponent->IsAttackInProgress() : false;
				if (!isMeleeAttacking)
				{
					const auto rangedComponent = PlayerController->GetRangedAttackComponent();
					if (rangedComponent) RangedAttack(PlayerController, playerCharacter, rangedComponent, CurrentTarget.Get(), AttackRadius);
				}
				else if (bHasRangedPressed) {
					ReleasePlayerAttackButton(PlayerController);
				}
			}

			if (meleeComponent) MeleeAttack(PlayerController, playerCharacter, meleeComponent, CurrentTarget.Get());
		}
		else if (bHasMeleePressed || bHasRangedPressed) {
			ReleasePlayerAttackButton(PlayerController);
		}
		else {
			PlayerAutomator->MoveToLocation(Location);
		}
	}

	return true;
}

void PlayerAttackRadiusAction::DebugDraw()
{
	Super::DebugDraw();

#if !UE_BUILD_SHIPPING
	const auto world = PlayerAutomator->GetWorld();
	const auto player = GetPlayerCharacter();

	DrawDebugCircle(world, player->GetActorLocation(), AttackRadius, 35.f, FColor::Orange, false, -1, 0, 5.f, FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f));
#endif
}

void PlayerAttackRadiusAction::SetTarget(ABasePlayerController* PlayerController, TWeakObjectPtr<ABaseCharacter> Target)
{
	Super::SetTarget(PlayerController, Target);
	if (CurrentTarget.IsValid()) {
		Location = CurrentTarget->GetActorLocation();
	}
}

bool PlayerAttackRadiusAction::ShouldTargetNewActor(const APlayerCharacter* player, TWeakObjectPtr<ABaseCharacter> target, float radius)
{
	if (IsValidTarget(player, target))
	{
		const auto isInRange = actorquery::is::inRange(player, radius)(target.Get());
		return !isInRange;
	}

	return true;
}

const auto ConeRadians = FMath::DegreesToRadians(360.f);

ABaseCharacter* PlayerAttackRadiusAction::GetClosestCharacter(AActor* source, float radius, int sourceActorID /* = INDEX_NONE */)
{
	if (source->IsPendingKillOrUnreachable()) {
		return nullptr;
	}

	auto playerSource = Cast<APlayerCharacter>(source);
	const auto predicate = [&](const AActor* target) {
		if (auto character = Cast<ABaseCharacter>(target)) {
			if (character->GetUniqueID() == sourceActorID) {
				return false;
			}
			return IsValidTarget(playerSource, character);
		}
		return false;
	};

	auto target = actorquery::getClosestActorInCone2D(source, ABaseCharacter::StaticClass(), radius, ConeRadians, true, predicate);
	if (target == nullptr) {
		float closestDistanceSquared;
		target = actorquery::getClosestActor(source, radius, ABaseCharacter::StaticClass(), closestDistanceSquared, true, predicate);
	}
	if (target == nullptr) return nullptr;
	return Cast<ABaseCharacter>(target);
}
