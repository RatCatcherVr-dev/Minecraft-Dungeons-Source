#include "Dungeons.h"
#include "BlazeFire.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/mob/MobCharacter.h"

ABlazeFire::ABlazeFire(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{	
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = .25f;
	bReplicates = true;	
}

void ABlazeFire::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	const auto location = GetActorLocation();

	for (auto mob : InstanceTracker<AMobCharacter>::GetList(GetWorld())) {
		if (mob->EntityType != EntityType::Blaze) {
			continue;
		}

		const auto mobLocation = mob->GetActorLocation();

		if (
			FMath::Abs(location.X - mobLocation.X) < radius &&
			FMath::Abs(location.Y - mobLocation.Y) < radius
		) {
			const auto healthComponent = mob->GetHealthComponent();
			healthComponent->ApplyHeal(healthComponent->GetMaximumHealth() * healFraction);
		}
	}
}
