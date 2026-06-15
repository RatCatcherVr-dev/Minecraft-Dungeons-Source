#include "Dungeons.h"
#include "EnderPearlInstance.h"
#include "Dungeons/game/actor/item/EnderPearlItem.h"
#include "Dungeons/game/actor/character/player/PlayerCharacter.h"
#include "Dungeons/game/actor/character/player/BasePlayerController.h"
#include <UnrealNetwork.h>


int AEnderPearlInstance::GetDisplayCount() const {
	return 0;
}

void AEnderPearlInstance::Activate(const FPredictionKey& predictionKey) {
	if (Role == ROLE_SimulatedProxy) {
		Super::Activate(predictionKey);
		return;
	}

	auto role = Role;
	AActor * owner = GetOwner();
	UWorld *world = GetWorld();
	FVector spawnPosition = owner->GetActorLocation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const auto& type = game::item::type::EnderPearl;
	auto item = world->SpawnActor<AEnderPearlItem>(ClassToSpawn, spawnPosition, FRotator::ZeroRotator, spawnParameters);
	if (item == nullptr)
		return;
	auto player = Cast<APlayerCharacter>(GetOwner());
	if (player) {
		FVector velocity = CalculateVelocityAmountToLoc(Target, 10.0f);
		auto collisionMesh = item->FindComponentByClass<USphereComponent>();
		collisionMesh->AddImpulse(velocity, NAME_None, true);
	}

	item->SetOwner(GetOwner());
	Super::Activate(predictionKey);
}

void AEnderPearlInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEnderPearlInstance, MaxDistance);
}
