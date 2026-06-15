#include "Dungeons.h"
#include "EffectGroup.h"
#include "DrawDebugHelpers.h"
#include "ReusedEffectActor.h"
#include "util/Algo.hpp"

void UEffectGroup::SpawnEffect(TSubclassOf<AReusedEffectActor> actorClass, const FTransform& transform) {
	SpawnedActors.RemoveAllSwap(RETLAMBDA(!it.Actor.IsValid()));
	const auto closestEffect = algo::min_element_by(SpawnedActors, RETLAMBDA(FVector::DistSquared(transform.GetLocation(), it.Actor->GetActorLocation())));

	if (closestEffect && (FVector::DistSquared(closestEffect->Actor->GetActorLocation(), transform.GetLocation()) < FMath::Square(Parameters.IndividualSpawnRadius))) {
		closestEffect->Actor->ResetEffect(closestEffect->InitialLifeSpan);
		if (!Parameters.bSpawnAtOldPositionIfOverlap) {
			closestEffect->Actor->SetActorTransform(transform);
		}
	}
	else {
		auto actor = GetWorld()->SpawnActor<AReusedEffectActor>(actorClass, transform);
		SpawnedActors.Emplace(actor, actor->GetLifeSpan());
	}

	if (Parameters.bDebugVisuals) {
		DrawDebugVisuals();
	}
}

void UEffectGroup::SetParams(FEffectGroupParams params) {
	Parameters = std::move(params);
}

void UEffectGroup::DrawDebugVisuals() {
	for (auto actor : SpawnedActors) {
		DrawDebugSphere(GetWorld(), actor.Actor->GetActorLocation(), 10.f, 50, FColor::Black, false, actor.InitialLifeSpan);
		DrawDebugSphere(GetWorld(), actor.Actor->GetActorLocation(), Parameters.IndividualSpawnRadius, 50, FColor::Purple, false, actor.InitialLifeSpan);
	}
}
