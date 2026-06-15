#include "SnapToGridComponent.h"
#include "GameFramework/Actor.h"
#include "game/Conversion.h"
#include "world/level/BlockPos.h"
#include "game/util/ActorQuery.h"
#include "game/GameBP.h"
#include "lovika/world/level/ChunkBlockSource.h"

USnapToGridComponent::USnapToGridComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USnapToGridComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* owner = GetOwner();

	auto Game = actorquery::getFirstActor<AGameBP>(GetWorld());
	auto blockSource = Game->BlockSource();

	FVector actorLocation = owner->GetActorLocation();
	FVector blockLoctation = conversion::blockToUe(conversion::ueToBlock(actorLocation));
	blockLoctation += FVector(50, 50, 50); //move from corner to center
	
	owner->SetActorLocation(blockLoctation);

	FRotator rotation = owner->GetActorRotation();
	rotation = rotation.GridSnap(FRotator(0,90,0));
	owner->SetActorRotation(rotation);

}

