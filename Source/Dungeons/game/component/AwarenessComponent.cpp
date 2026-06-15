#include "Dungeons.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include <EngineUtils.h>
#include "AwarenessComponent.h"


UAwarenessComponent::UAwarenessComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UAwarenessComponent::BeginPlay() {
	Super::BeginPlay();
	const auto childActorComponents = GetOwner()->GetComponentsByTag(UChildActorComponent::StaticClass(), FName { "awareness-volume" });

	if (childActorComponents.Num() > 0) {
		child = Cast<UChildActorComponent>(childActorComponents[0])->GetChildActor();
	}
}

void UAwarenessComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction* thisTickFunction) {
	const auto world = GetWorld();

	const auto now = world->GetTimeSeconds();
	if (now - lastTime > .25f) {
		lastTime = now;

		const auto ownerLocation = GetOwner()->GetActorLocation();
		const auto triggerRangeSquared = FMath::Square(awareRange);
		const auto closeRangeSquared = FMath::Square(fraction * maxRange);
		
		const auto wasAware = isAware;
		isAware = false;
		for (auto&& player : TActorRange<APlayerCharacter>(world)) {
			const auto distanceSquared = FVector::DistSquared2D(ownerLocation, player->GetActorLocation());
			if (distanceSquared < triggerRangeSquared) {
				isAware = true;

				if (distanceSquared < closeRangeSquared) {
					isTriggered = true;
					SetComponentTickEnabled(false);
					OnTriggered.Broadcast();
				}

				break;
			}
		}

		if (wasAware && !isAware) {
			OnContract.Broadcast();
		} else if (!wasAware && isAware) {
			OnExpand.Broadcast();
		}
	}

	if (isAware && fraction < 1.f) {
		if (fraction <= 0.f) {
			child->SetActorHiddenInGame(false);
			OnAwareBegin.Broadcast();
		}

		fraction = FMath::Min(fraction + expandRate * deltaTime, 1.f);
		child->SetActorScale3D(FVector { fraction * maxScale });

		if (debugView) {
			DrawDebugSphere(world, GetOwner()->GetActorLocation(), fraction * maxRange, 16, FColor::White);
		}
	} else if (!isAware && fraction > 0.f) {
		fraction -= contractRate * deltaTime;

		if (fraction <= 0.f) {
			fraction = 0.f;
			child->SetActorHiddenInGame(true);
			OnAwareEnd.Broadcast();
		} else {
			child->SetActorScale3D(FVector { fraction * maxScale });
			
			if (debugView) {
				DrawDebugSphere(world, GetOwner()->GetActorLocation(), fraction * maxRange, 16, FColor::White);
			}
		}
	}
}

float UAwarenessComponent::GetFraction () const {
	return fraction;
}

bool UAwarenessComponent::IsTriggered() const {
	return isTriggered;
}

bool UAwarenessComponent::IsAware() const {
	return isAware;
}

void UAwarenessComponent::ForceTrigger() {
	isTriggered = true;
	SetComponentTickEnabled(false);
	OnTriggered.Broadcast();
}