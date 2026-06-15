#include "Dungeons.h"
#include "game/util/ActorQuery.h"
#include "DistancelimitComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"

UDistancelimitComponent::UDistancelimitComponent() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f;
}

void UDistancelimitComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	const float distanceMaxSquared = FMath::Square(3000.f);
	const auto ownLocation = GetOwner()->GetActorLocation();

	const auto players = actorquery::getActors<APlayerCharacter>(GetWorld());

	for (auto player : players) {
		if (FVector::DistSquared2D(player->GetActorLocation(), ownLocation) < distanceMaxSquared) {
			return;
		}
	}

	GetOwner()->Destroy();
}

