#include "Dungeons.h"
#include "SecretComponent.h"
#include "DungeonsGameMode.h"


USecretComponent::USecretComponent()
{
}

void USecretComponent::SecretFound(AActor* SecretFinder)
{
	if (GetOwner()->HasAuthority() && !isFound) {
		ExecuteSecretFound(SecretFinder);
		isFound = true;
	}
}

void USecretComponent::BeginPlay()
{
	Super::BeginPlay();

	if (auto* gameMode = GetWorld()->GetAuthGameMode<ADungeonsGameMode>()) {
		gameMode->OnSecretSpawned(*this);
	}
}

void USecretComponent::ExecuteSecretFound_Implementation(AActor* SecretFinder)
{
	OnSecretFoundRecieved.Broadcast();
	OnSecretFound.Broadcast(SecretFinder);
}

