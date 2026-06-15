#pragma once

#include <Containers/Array.h>
#include <Engine/EngineTypes.h>


class AActor;
class APlayerController;

struct RayActorHit {
	TWeakObjectPtr<AActor> Actor;
	float Distance;

	RayActorHit(TWeakObjectPtr<AActor> actor = TWeakObjectPtr<AActor>(), float distance = FLT_MAX)
		: Actor(actor)
		, Distance(distance)
	{}
};


struct TargetSelection {
	TArray<RayActorHit> mActorCandidates;
	FVector mOrigin;
	FVector mDirection;
	RayActorHit mClosestMobUnderCursor;
	RayActorHit mClosestTerrain;
	RayActorHit mClosestClicky;
};


TArray<RayActorHit> getActorsFromHitResults(APlayerController* playerController, const FVector2D& screenSpaceOrigin, const TArray<FHitResult>& hitResults);

TArray<RayActorHit> castRay(UWorld* world, APlayerController* playerController, ECollisionChannel channel, const FVector2D& screenSpaceOrigin, const FVector& origin, const FVector& direction, float maxDistance = 1000000.0f, float radius = 0.0f);

TargetSelection castRayTargetSelect(UWorld* world, APlayerController* playerController, const FVector2D& screenSpaceOrigin, float maxDistance, float radius = 0.0f);