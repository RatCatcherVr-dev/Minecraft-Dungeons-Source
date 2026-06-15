#include "Dungeons.h"
#include "ChestActor.h"
#include <UnrealNetwork.h>

void AChestActor::BeginPlay() {
	Super::BeginPlay();
}

bool AChestActor::getIsOpened() const
{
	return bOpened;
}

bool AChestActor::getIsDiscovered() const
{
	return bDiscovered;
}

EChestType AChestActor::getChestType() const
{
	return mChestType;
}

void AChestActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);	
	DOREPLIFETIME(AChestActor, bOpened);
}

