
#include "Dungeons.h"
#include "TargetController.h"

#include <WeakObjectPtrTemplates.h>
#include "game/actor/character/mob/MobCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/abilities/cues/DungeonsGameplayCueNotify_Actor.h"
#include "game/component/InteractableComponent.h"
#include <GameFramework/PlayerController.h>
#include "game/actor/character/player/BasePlayerController.h"
#include "game/actor/item/StorableItem.h"
#include "DungeonsGameInstance.h"
#include "game/actor/item/StorableItem.h"
#include "MouseButtonClickyState.h"
#include "game/team/TeamQuery.h"
#include <utility>

DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_UpdateRadialTargetSelection_OverlapMultiByChannel"), STAT_ABasePlayerController_UpdateRadialTargetSelection_OverlapMultiByChannel, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_UpdateRadialTargetSelection_ProcessResults"),STAT_ABasePlayerController_UpdateRadialTargetSelection_ProcessResults, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_UpdateRadialTargetSelection_UpdateCurrentTaget"),STAT_ABasePlayerController_UpdateRadialTargetSelection_UpdateCurrentTaget, STATGROUP_PlayerController);

DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_TargetController_Update"), STAT_ABasePlayerController_TargetController_Update, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_TargetController_Update_mHighlightController"), STAT_ABasePlayerController_TargetController_Update_mHighlightController, STATGROUP_PlayerController);
DECLARE_CYCLE_STAT(TEXT("STAT_ABasePlayerController_TargetController_Update_IsTargetTargetable"), STAT_ABasePlayerController_TargetController_Update_IsTargetTargetable, STATGROUP_PlayerController);

namespace target {

using TargetScore = std::pair<int, float>;

TargetScore TargetScorer(AActor* target, APlayerCharacter* player) {
	const auto prio = [&]() {
		const auto* storable = Cast<AStorableItem>(target);
		if (storable && (storable->GetItemType().isGear() || storable->GetItemType().isPermanent())) {
			return player->GetItemStashComponent()->IsInventoryFull() ? 100 : 0;
		}
		return 1;
	}();
	
	return std::make_pair(prio, FVector::DistSquared2D(target->GetActorLocation(), player->GetActorLocation()));
}

}

void TargetController::Update() {
	if (CurrentTarget.IsValid() && !IsTargetTargetable(CurrentTarget.Get())) {
		ResetTarget();
	}
}

void TargetController::SetTargetType(ETargetType type) {
	TargetType = type;

	// D11.DB - If we're using a gamepad then we never want to use TargetOnly as that is now handled by the autotargeting system.
	if (CachedPlayerController.IsValid() && type == ETargetType::TargetOnly && CachedPlayerController->GetGamepadActive()) {
		return;
	}

	switch (type)
	{
	case TargetController::ETargetType::None:
		if (CurrentTarget.IsValid()) ResetTarget();
		if (TargetCandidate.IsValid()) TargetCandidate.Reset();
		break;
	case TargetController::ETargetType::TargetOnly:
		if (CurrentTarget.IsValid() && IsClickyTarget(CurrentTarget.Get())) ResetTarget();
		if (TargetCandidate.IsValid() && IsClickyTarget(TargetCandidate.Get())) TargetCandidate.Reset();
		break;
	case TargetController::ETargetType::ClickyOnly:
		if (CurrentTarget.IsValid() && !IsClickyTarget(CurrentTarget.Get())) ResetTarget();
		if (TargetCandidate.IsValid() && !IsClickyTarget(TargetCandidate.Get())) TargetCandidate.Reset();
		break;
	case TargetController::ETargetType::All:
	default:
		break;
	}
}

void TargetController::SetTargetMode(ETargetMode mode) {
	TargetMode = mode;
	UpdateCurrentTaget();
}

FVector TargetController::GetClosestTerrain() const {
	return mTargetSelection.mOrigin + mTargetSelection.mDirection * mTargetSelection.mClosestTerrain.Distance;
}

void TargetController::ResetTarget() {
	UpdateCurrentTarget(nullptr);
}

void TargetController::ResetTargetCandidate() {	
	TargetCandidate.Reset();
}

void TargetController::HighlightTarget(AActor* actor) {
	if (auto clicky = UInteractableComponent::GetComponentFromActor(actor)) {
		if (CachedPlayerController != nullptr)
		{
			clicky->BeginHighlight(CachedPlayerController->GetControlledPlayerCharacter());
		}
	}
	else {

		if(auto mob = Cast<AMobCharacter>(actor))
		{
			if (CachedPlayerController != nullptr)
			{
				mob->cachedTargetees.Add(CachedPlayerController->GetControlledPlayerCharacter());
			}
		}

		if (AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(actor->GetWorld()))
		{
			highlightController->BeginOutlineActor(CachedPlayerController->GetControlledPlayerCharacter(), actor);
		}
	}
}

void TargetController::UnhighlightTarget(AActor* actor) {
	if (auto clicky = UInteractableComponent::GetComponentFromActor(actor)) {
		if (CachedPlayerController != nullptr)
		{
			clicky->EndHighlight(CachedPlayerController->GetControlledPlayerCharacter());
		}
	}
	else {
		if (AHighlightController* highlightController = AHighlightController::CheckHighlightControllerExists(actor->GetWorld()))
		{
			highlightController->EndOutlineActor(actor);

			if (auto mob = Cast<AMobCharacter>(actor))
			{
				if (CachedPlayerController != nullptr)
				{
					mob->cachedTargetees.Remove(CachedPlayerController->GetControlledPlayerCharacter());
				}

				if (mob->cachedTargetees.Num() > 0)
				{
					for (auto& targeter : mob->cachedTargetees)
					{
						if (targeter.IsValid() && !targeter.IsStale())
						{
							if (auto targeterController = Cast<ABasePlayerController>(targeter->GetPlayerController()))
							{
								//Quick reset on the outline.
								highlightController->EndOutlineActor(mob);
								highlightController->BeginOutlineActor(targeter.Get(), mob);
								break;
							}
						}
					}
				}
			}
		}
	}
}

bool TargetController::IsClickyTarget(const AActor* actor) const {
	return UInteractableComponent::GetComponentFromActor(actor) != nullptr;
}

bool TargetController::IsTargetTargetable(const AActor* actor) const {
	if (!CachedPlayerController.IsValid()) {
		return false;
	}

	if (CachedPlayerController->GetControlledPlayerCharacter()->IsFrozenSolid()) {
		if (auto IceCube = Cast<AFrozenSolidIceCubeActor>(actor)) {
			return IceCube->GetOwner() == CachedPlayerController->GetCharacter();
		}
		return false;
	}

	if (auto mob = Cast<AMobCharacter>(actor)) {	
		float heightDifference = CachedPlayerController->GetCharacter()->GetActorLocation().Z - mob->GetActorLocation().Z;
		if (heightDifference > 2000.0f)	{
			return false;
		}
		else {
			return (TargetType == ETargetType::All || TargetType == ETargetType::TargetOnly) && actorquery::isActorTargetableByPlayer(mob) && actorquery::is::alive(mob);
		}
	}	

	if (auto pickup = Cast<AStorableItem>(actor)) {
		return !pickup->lockItemToOwner || pickup->GetOwner() == CachedPlayerController->GetCharacter();
	}

	return (TargetType == ETargetType::All || TargetType == ETargetType::ClickyOnly) && UInteractableComponent::GetComponentFromActor(actor);
}

bool TargetController::IsTargetClickyTargetable(UInteractableComponent* Clicky, APlayerCharacter* playerCharacter) const
{
	if (Clicky && playerCharacter)
	{		
		auto* pOwner = Clicky->GetCachedOwner();
		if (auto pickup = Cast<AStorableItem>(pOwner)) {
			return !pickup->lockItemToOwner || pickup->GetOwner() == playerCharacter;
		}

		return (TargetType == ETargetType::All || TargetType == ETargetType::ClickyOnly);
	}

	return false;
}

void TargetController::UpdateCursorTargetSelection(UWorld* world, ABasePlayerController* playerController, const FVector2D& screenSpaceOrigin, float maxDistance, float radius) {
	CachedPlayerController = playerController;

	//D11.KS - We should use the local main players screen as local players technically don't have a screen.
	APlayerController* mainPlayerController = world->GetGameInstance()->GetFirstLocalPlayerController(world);
	mTargetSelection = castRayTargetSelect(world, mainPlayerController, screenSpaceOrigin, maxDistance, radius);
	//mTargetSelection = castRayTargetSelect(world, playerController, screenSpaceOrigin, maxDistance, radius );

	auto PlayerCharacter = playerController->GetControlledPlayerCharacter();

	TargetCandidate.Reset();

	if (mTargetSelection.mClosestMobUnderCursor.Actor.IsValid() && IsTargetTargetable(mTargetSelection.mClosestMobUnderCursor.Actor.Get())) 
	{
		if (auto character = Cast<ABaseCharacter>(mTargetSelection.mClosestMobUnderCursor.Actor.Get()))
		{
			UHealthComponent* healthComponent = character->GetHealthComponent();
			check(healthComponent && "Mobs should always have a health component");
			if (healthComponent->IsAlive())
			{
				TargetCandidate = mTargetSelection.mClosestMobUnderCursor.Actor.Get();
			}
		}
	}
	else if (mTargetSelection.mClosestClicky.Actor.IsValid() && IsTargetTargetable(mTargetSelection.mClosestClicky.Actor.Get())) 
	{
		//D11.SC DOnt allow selection of the players own character
		if (mTargetSelection.mClosestClicky.Actor != PlayerCharacter)
		{
			TargetCandidate = mTargetSelection.mClosestClicky.Actor.Get();
		}
	}

	UpdateCurrentTaget();
}

// D11.DB
void TargetController::UpdateRadialTargetSelection(APlayerCharacter* playerCharacter, float DeltaTime) {

	if (fUpdateDelay <= 0.0f)
	{
		fUpdateDelay = 0.1f;
	}
	else
	{
		fUpdateDelay -= DeltaTime;
		return;
	}

	// D11.DB - Use the player controller's character location and the provided radius to perform sphere intersection
	//			tests on all UInteractableComponent instances.
	TargetCandidate.Reset();

	//Find the closest candidate from the potential user map
	if (playerCharacter) {
		UWorld* pWorld = playerCharacter->GetWorld();

		// #D11.CM
		CachedPlayerController = playerCharacter->GetPlayerController();

		const FVector playerLocation = playerCharacter->GetActorLocation();

		auto& InteractableComponentList = InstanceTracker< UInteractableComponent >::GetList(pWorld);

		AActor* bestResult = nullptr;
		target::TargetScore bestScore(100000, BIG_NUMBER);
			
		if(CanPlayerInteractWithClickies(playerCharacter)) {

			SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_UpdateRadialTargetSelection_ProcessResults);

			{
				if (playerCharacter->IsFrozenSolid())
				{
					TargetCandidate = playerCharacter->GetCurrentIceCube();
				}
				else
				{
					const bool bGamePadActive = (CachedPlayerController.IsValid() && CachedPlayerController->GetGamepadActive());

					// If we're not frozen, find downed players first
					auto& PlayerInteractionList = InstanceTracker< APlayerCharacter >::GetList(pWorld);

					for (APlayerCharacter* pPlayer : PlayerInteractionList)
					{
						if (pPlayer == playerCharacter) //not this character
							continue;

						auto ClickyComponent = pPlayer->GetPlayerInteractableComponent();

						if (ClickyComponent->IsInteractionEnalbed())
						{
							const FVector actorLocation = pPlayer->GetActorLocation();
							float objectRadius = ClickyComponent->GetRadius();

							if (bGamePadActive)
							{
								// D11.DB - This allows the radius to be tweaked per interactable for gamepad controls.
								objectRadius += ClickyComponent->GetRadiusGamepadModifier();
							}

							const float validDistanceSquared = FMath::Square(objectRadius);

							const float distanceSquared = FVector::DistSquared(actorLocation, playerLocation);

							if (distanceSquared < validDistanceSquared && distanceSquared < bestScore.second)
							{
								bestScore.second = distanceSquared;
								bestResult = pPlayer;
							}
						}
					}
					

					static constexpr auto interactionZRange = 700.f;
					//only do the items if we havent got a downed player close

					if (!bestResult) {
						for (auto ClickyComponent : InteractableComponentList) {
							if (ClickyComponent->IsInteractionEnalbed()) {

								if (!IsTargetClickyTargetable(ClickyComponent, playerCharacter))
								{
									continue;
								}

								// #D11.Cm
								if (ClickyComponent->bOneInteractionPerPlayer && ClickyComponent->HasBeenUsedBy(playerCharacter)) {
									continue;
								}

								float objectRadius = ClickyComponent->GetRadius();

								if (bGamePadActive)
								{
									// D11.DB - This allows the radius to be tweaked per interactable for gamepad controls.
									objectRadius += ClickyComponent->GetRadiusGamepadModifier();
								}

								// D11.DB - We use validDistance to reject some entities and also
								//			tweak their radii.
								const float validDistanceSquared = FMath::Square(objectRadius);

								AActor* pTargetActor = ClickyComponent->GetCachedOwner();

								// D11.DB - Track the object if it's the closest one yet and it succeeds the validDistance check.
								//			Otherwise we can ignore it.
								const float ZDistance = FMath::Abs(pTargetActor->GetActorLocation().Z - playerLocation.Z);

								const auto targetScore = target::TargetScorer(pTargetActor, playerCharacter);

								if (targetScore.second < validDistanceSquared && ZDistance < interactionZRange) {
									if (!bestResult || (targetScore < bestScore)) {
										bestScore = targetScore;
										bestResult = pTargetActor;
									}
								}
							}
						}
					}
				}
			}
		}
			
		if (bestResult)
		{
			TargetCandidate = bestResult;
		}
		
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_ABasePlayerController_UpdateRadialTargetSelection_UpdateCurrentTaget);
		UpdateCurrentTaget();
	}
}


void TargetController::UpdateCurrentTaget() {
	bool isNewTarget = TargetCandidate != CurrentTarget;

	if (!isNewTarget) return;

	bool shouldChangeTarget = false;

	switch (TargetMode)
	{
	case TargetController::ETargetMode::Default:
		shouldChangeTarget = true;
		break;
	case TargetController::ETargetMode::Sticky:
		shouldChangeTarget = false;
		break;
	case TargetController::ETargetMode::OnlyNew:
		shouldChangeTarget = TargetCandidate.IsValid() && IsTargetTargetable(TargetCandidate.Get());
		break;
	case TargetController::ETargetMode::StickyReplaceEmpty:
		shouldChangeTarget = TargetCandidate.IsValid() && IsTargetTargetable(TargetCandidate.Get()) && !CurrentTarget.IsValid(); 
		break;
	default:
		break;
	}

	if (shouldChangeTarget) {
		AActor* newTarget = TargetCandidate.Get();
		UpdateCurrentTarget(newTarget);
	}
}

void TargetController::UpdateCurrentTarget(AActor* newTarget) {
	AActor* oldTarget = CurrentTarget.Get();
	if (CurrentTarget.IsValid()) UnhighlightTarget(CurrentTarget.Get());
	
	CurrentTarget = newTarget;

	if (CurrentTarget.IsValid()) HighlightTarget(CurrentTarget.Get());

	if (CachedPlayerController.IsValid() && newTarget != oldTarget) {
		CachedPlayerController->OnHighlightTargetChanged(newTarget, oldTarget);
	}
}

// #D11.CM
bool TargetController::CanPlayerInteractWithClickies(APlayerCharacter* playerCharacter) {
	if (!CachedPlayerController.IsValid()) return true;
	if (!CachedPlayerController->GetGamepadActive()) return true;
	if (!CachedPlayerController->GetAutoTargetSystem()->IsTargetingStickActive(*CachedPlayerController.Get())) return true;
	

	// We don't include quick grow vines in these checks due to their frequent proximity to vital interactions.
	// Do our close range check first.
	if (auto meleeAttackComponent = CachedPlayerController->GetMeleeAttackComponent()) {
		if (meleeAttackComponent->IsAttackInProgress()) {
			return false;
		}
	}

	float distSquared = 0;
	const float directionalInteractCutoffSqr = 450 * 450;
	const FVector playerLocation = playerCharacter->GetActorLocation();

	// Now check our targets from the targeting system.
	if (auto autoTarget = CachedPlayerController->GetAutoTargetSystem()) {
		for (auto targetOrder : autoTarget->GetTargets()) {
			auto target = targetOrder.Mob;
			if (target.IsValid() && teamquery::can::damage(ETeamName::Heroes, target.Get()->GetCurrentTeam()) && target.Get()->EntityType != EntityType::QuickGrowingVine && target.Get()->EntityType != EntityType::QuickGrowingVineSimple) {
				distSquared = FVector::DistSquared(playerLocation, target->GetActorLocation());
				if (distSquared < directionalInteractCutoffSqr) {
					if (FVector::DotProduct(playerCharacter->GetActorForwardVector(), target->GetActorLocation() - playerLocation) > 0.0f) {
						return false;
					}
				}
			}
		}
	}

	return true;
}

void TargetController::ForceSetTarget(AActor* actor) {
	if (IsTargetTargetable(actor)) {		
		UpdateCurrentTarget(actor);		
	}
}