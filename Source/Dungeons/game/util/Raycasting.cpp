
#include "Dungeons/Dungeons.h"
#include "Raycasting.h"

#include "game/util/ActorQuery.h"
#include "game/actor/character/mob/MobCharacter.h"
#include "game/component/InteractableComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include <Engine/World.h>
#include <GameFramework/HUD.h>
#include <GameFramework/PlayerController.h>
#include "Engine/Engine.h"

float screenSpaceSquareDistance(APlayerController* playerController, const FVector2D& mousePosition, AActor* actor) {
	FVector2D actorPosition;
	UGameplayStatics::ProjectWorldToScreen(playerController, actor->GetActorLocation(), actorPosition);
	return FVector2D::DistSquared(mousePosition, actorPosition);
}

TArray<RayActorHit> getActorsFromHitResults(APlayerController* playerController, const FVector2D& screenSpaceOrigin, const TArray<FHitResult>& hitResults)
{
	TArray<RayActorHit> actors;

	// Check if there is a targetable mob, item or clicky without a ClickCollider
	bool foundOnlyNearMisses = nullptr == hitResults.FindByPredicate([](auto hit) {
		if (actorquery::isActorTargetableByPlayer(hit.Actor) || hit.Actor->template FindComponentByClass<UInteractableComponent>() != nullptr) {
			auto response = hit.Component->GetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::NearMissTargetSelect);
			if (response == ECollisionResponse::ECR_Ignore) {
				return true;
			}
		}
		return false;
	});

	for (auto hit : hitResults) {
		if (!hit.GetActor()->IsActorInitialized()) {
			continue;
		}
		if (!foundOnlyNearMisses) {
			auto response = hit.Component->GetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::NearMissTargetSelect);
			if (response != ECollisionResponse::ECR_Ignore) {
				continue;
			}
		}
		if (actors.Num() == 0 || actors.Last().Actor != hit.GetActor()) {
			//If it's a direct hit (foundOnlyNearMisses is false), we're already excluding any near misses, and we want to sort
			//any objects hit by z-value to get the one the mouse is directly hovering. If it's an indirect hit (foundOnlyNearMisses is true),
			//we want to sort those objects by screen space distance to the cursor instead.
			float distance = (foundOnlyNearMisses ? screenSpaceSquareDistance(playerController, screenSpaceOrigin, hit.GetActor()) : hit.Distance);
			actors.Add(RayActorHit(hit.GetActor(), distance));
		}
	}

	return actors;
}


RayActorHit getClosestMobOrClickCollider(const TArray<FHitResult>& hitResults)
{
	RayActorHit clickCollider;

	for (auto hit : hitResults) {
		if (hit.Actor->IsA<AMobCharacter>()) {
			auto response = hit.Component->GetCollisionResponseToChannel((ECollisionChannel)ECustomTraceChannels::NearMissTargetSelect);
			if (response == ECollisionResponse::ECR_Ignore && hit.Actor->IsA<AMobCharacter>())
				return RayActorHit(hit.GetActor(), hit.Distance);
			if (response != ECollisionResponse::ECR_Ignore && clickCollider.Distance > hit.Distance)
				clickCollider = RayActorHit(hit.GetActor(), hit.Distance);
		}
	}

	return clickCollider;
}


TArray<RayActorHit> castRay(UWorld* world, APlayerController* playerController, ECollisionChannel channel, const FVector2D& screenSpaceOrigin, const FVector& origin, const FVector& direction, float maxDistance, float radius)
{
	TArray<FHitResult> hitResults;
	TArray<RayActorHit> actors;

	if (world == nullptr)
		return actors;

	const auto end = origin + direction * maxDistance;

	// If end is towards infinity LineTraceMultiByChannel fail to find any hits at all
	if (radius < 5.0f) {
		world->LineTraceMultiByChannel(hitResults, origin, end, channel);
	}
	else {
		// D11.DB - Sometimes we might want a wide ray to catch more entities.
		world->SweepMultiByChannel(hitResults, origin, end, FQuat::Identity, channel, FCollisionShape::MakeSphere(radius));
	}
	return getActorsFromHitResults(playerController, screenSpaceOrigin, hitResults);
}

TargetSelection castRayTargetSelect(UWorld* world, APlayerController* playerController, const FVector2D& screenSpaceOrigin, float maxDistance, float radius) {
	FVector origin, direction;
	UGameplayStatics::DeprojectScreenToWorld(playerController, screenSpaceOrigin, origin, direction);

	TargetSelection ts;

	ts.mActorCandidates = castRay(world, playerController, (ECollisionChannel)ECustomTraceChannels::TargetSelect, screenSpaceOrigin, origin, direction, maxDistance, radius);
	ts.mOrigin = origin;
	ts.mDirection = direction;

	for (auto hit : ts.mActorCandidates) {
		const auto ClickyComponent = hit.Actor->FindComponentByClass<UInteractableComponent>();	
		
		if (ts.mClosestMobUnderCursor.Distance > hit.Distance && actorquery::isActorTargetableByPlayer(hit.Actor)) {
			ts.mClosestMobUnderCursor = hit;
		}

		else if (ts.mClosestClicky.Distance > hit.Distance && ClickyComponent != nullptr && ClickyComponent->IsInteractionEnalbed()) {
			if (!ClickyComponent->bOneInteractionPerPlayer || !ClickyComponent->HasBeenUsedBy(Cast<ABasePlayerController>(playerController)->GetControlledPlayerCharacter())) {
				if (radius > 5.0f && ClickyComponent->GetType() == EClickyEnum::CE_Item) {
					// D11.DB - Items need to be handled a little differently for playability reasons 
					//			as the player cannot collide with them.
					float objRadius = ClickyComponent->GetRadius();
						objRadius *= objRadius * 1.5f;

						if (hit.Distance < objRadius) {
							ts.mClosestClicky = hit;
						}
				}
				else {
					ts.mClosestClicky = hit;
				}
			}
		}

		else if (ts.mClosestTerrain.Distance > hit.Distance && actorquery::isActorTerrain(hit.Actor)) {
			ts.mClosestTerrain = hit;
		}
	}

	return ts;
}