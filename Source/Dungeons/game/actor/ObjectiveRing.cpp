#include "Dungeons.h"
#include "ObjectiveRing.h"

AObjectiveRing::AObjectiveRing(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = false;	
}

void AObjectiveRing::BeginPlay() {
	Super::BeginPlay();	

	fraction = 0.f;
}

void AObjectiveRing::SetPath(const FVector& _source, const FVector& _target) {
	source = _source;
	target = _target;

	fraction = 0.f;

	SetActorLocation(source);
	SetActorTickEnabled(true);
}

void AObjectiveRing::Tick(float deltaTime) {
	Super::Tick(deltaTime);

	fraction += deltaTime * speed;
	if (fraction > 1.f) {
		Destroy();
	}

	SetActorLocation(FMath::Lerp(source, target, fraction));
}