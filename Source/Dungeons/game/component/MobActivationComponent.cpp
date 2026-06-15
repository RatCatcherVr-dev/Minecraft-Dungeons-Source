#include "Dungeons.h"
#include "MobActivationComponent.h"
#include "DungeonsGameMode.h"


UMobActivationComponent::UMobActivationComponent()
{
}

void UMobActivationComponent::ActivateMob()
{
	bIsMobActive = true;
	OnActivationChanged.Broadcast(bIsMobActive);
}

void UMobActivationComponent::DeactivateMob()
{
	bIsMobActive = false;
	OnActivationChanged.Broadcast(bIsMobActive);
}

bool UMobActivationComponent::IsMobActive()
{
	return bIsMobActive;
}
