#include "Dungeons.h"
#include "game/actor/item/SplashSlowingPotionItem.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "SplashSlowingPotionInstance.h"

void ASplashSlowingPotionInstance::Activate(const FPredictionKey& predictionKey) {
	if (Role == ROLE_SimulatedProxy)
		return;
	auto role = Role;
	AActor * owner = GetOwner();
	UWorld *world = GetWorld();
	FVector spawnPosition = owner->GetActorLocation();

	FActorSpawnParameters spawnParameters;
	spawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const auto& type = game::item::type::SplashSlowingPotion;
	auto item = world->SpawnActor<ASplashSlowingPotion>(ClassToSpawn, spawnPosition, FRotator::ZeroRotator, spawnParameters);
	if (item == nullptr)
		return;
	auto player = Cast<APlayerCharacter>(GetOwner());
	if (player) {
		FVector velocity = CalculateVelocityAmountToLoc(Target, 200.0f);
		auto collisionMesh = item->FindComponentByClass<USphereComponent>();
		collisionMesh->AddImpulse(velocity, NAME_None, true);
	}

	item->SetOwner(GetOwner());
	Super::Activate(predictionKey);
}


