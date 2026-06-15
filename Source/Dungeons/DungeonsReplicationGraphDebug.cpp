#include "DungeonsReplicationGraphDebug.h"
#include "DrawDebugHelpers.h"

ADungeonsReplicationGraphDebug::ADungeonsReplicationGraphDebug()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADungeonsReplicationGraphDebug::BeginPlay()
{
	Super::BeginPlay();
	DrawDebugPoint(GetWorld(), FVector(0, 0, 600), 300, FColor(52, 220, 239), true);
}

void ADungeonsReplicationGraphDebug::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADungeonsReplicationGraphDebug::TestDraw(FVector const& pos, FVector const& extent)
{
	DrawDebugBox(GetWorld(), pos, extent, FColor::Magenta);
	//DrawDebugPoint(GetWorld(), FVector(0, 0, 600), 300, FColor(52, 220, 239), true);
}
