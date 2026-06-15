#include "DarkSpawner.h"
#include "Dimmer.h"
#include <NavigationSystem.h>
 #include "game/util/LocationQuery.h"


ADarkSpawner::ADarkSpawner(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .25f;
	mDoPlayerDistanceCheck = false;
}

void ADarkSpawner::BeginPlay() {
	Super::BeginPlay();
}

void ADarkSpawner::TickSuddenDeath() {
	for (auto player : InstanceTracker<APlayerCharacter>::GetList(GetWorld())) {
		
	}
}

void ADarkSpawner::Tick(float DeltaTime) {
	if (!HasAuthority()) {
		return;
	}

	Super::Tick(DeltaTime);

	const auto world = GetWorld();
	if (!dimmer.IsValid()) {
		dimmer = actorquery::getFirstActor<ADimmer>(world);
	}
	if (dimmer == nullptr || !dimmer->IsNight()) {
		currentWaveSize = waveSizeStart;
		// just decelerate; don't actually spawn anything or update the fraction

		if (despawnActive) {
			if (mobs.Num() > 0) {
				const auto now = world->GetTimeSeconds();
				if (now - despawnAccumulator > despawnRate) {
					despawnAccumulator -= despawnRate;

					Despawn(mobs.Pop().Get()); // random enough?
				}
			}
		} else {
			despawnActive = true;
			despawnAccumulator = world->GetTimeSeconds();
		}

		if (!nightModeActive) {
			nightModeActive = true;
			nightModeTime = world->GetTimeSeconds();
		}

		return;
	}

	despawnActive = false;
	nightModeActive = false;

	TickSpawner(DeltaTime);
}

void ADarkSpawner::CalculateFraction(float DeltaTime)
{
	//Spawn rate scales with night fraction progress, the internal spawn rate increase and number of players alive.
 	fraction += dimmer->GetNightIntensity() * spawnRateMax * DeltaTime * NumPlayerAlive;
}

FVector ADarkSpawner::GetSpawnCentrePoint(AActor* actor) const
{
	return actor->GetActorLocation() + actor->GetVelocity() * (500.f / 700.f);
}
