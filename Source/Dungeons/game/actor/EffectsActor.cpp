#include "Dungeons.h"
#include "EffectsActor.h"
#include "game/util/ActorQuery.h"
#include "game/util/LocationQuery.h"

AEffectsActor::AEffectsActor(const FObjectInitializer& objectInitializer)
	: Super(objectInitializer)	
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetUpdateFrequency = 0.1f;
}

void AEffectsActor::BeginPlay() {
	//SetNetDormancy(DORM_Initial);
	Super::BeginPlay();	
}

void AEffectsActor::SpawnEffectsAtLocation(UParticleSystem* emitter, USoundCue* sound, const FVector& location) {
	if (HasAuthority()) {
		MulticastSpawnEffectsAtLocation(emitter, sound, location);
	} else {
		ServerSpawnEffectsAtLocation(emitter, sound, location);
	}
}

void AEffectsActor::ServerSpawnEffectsAtLocation_Implementation(UParticleSystem* emitter, USoundCue* sound, const FVector& location) {
	MulticastSpawnEffectsAtLocation(emitter, sound, location);
}

bool AEffectsActor::ServerSpawnEffectsAtLocation_Validate(UParticleSystem* emitter, USoundCue* sound, const FVector& location) {
	return true;
}

void AEffectsActor::MulticastSpawnEffectsAtLocation_Implementation(UParticleSystem* emitter, USoundCue* sound, const FVector& location) {
	const auto world = GetWorld();

	if (emitter != nullptr) {
		UGameplayStatics::SpawnEmitterAtLocation(world, emitter, location);
	}
	

	if (sound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(world, (USoundBase*)sound, location);
	}
}

void AEffectsActor::PreSpawnEffectsAtLocation(FString packName, bool isLarge, const FVector& location) {
	const auto& pack = packs[packName];	
	
	SpawnEffectsAtLocation(
		isLarge ? pack.PreSpawnLargeParticles : pack.PreSpawnMediumParticles,
		pack.PreSpawnSound,
		location
	);
}

void AEffectsActor::SpawnEffectsAtLocation(FString packName, bool isLarge, const FVector& location) {
	const auto& pack = packs[packName];	
	
	SpawnEffectsAtLocation(
		isLarge ? pack.SpawnLargeParticles : pack.SpawnMediumParticles,
		pack.SpawnSound,
		location
	);
}

void AEffectsActor::SpawnEffectsInRadius(UParticleSystem* emitter, USoundCue* sound, const FVector& location, float areaRadius, float instanceRadius) {
	auto& world = *GetWorld();

	for (auto radius = instanceRadius; radius < areaRadius; radius += instanceRadius * 2.f) {
		const auto circumference = radius * 2.f * PI;
		const int count = FMath::RoundToInt(circumference / (instanceRadius * 2.f)) + 1;
		const auto angleIncrement = (PI * 2.f) / (count + 1);
		const auto forward = FVector::ForwardVector * radius;

		auto angle { FMath::RandRange(0.f, PI) };
		for (auto i = 0; i <= count; i++) {
			const auto instanceLocation = location + forward.RotateAngleAxis(FMath::RadiansToDegrees(angle), FVector::UpVector);

			if (const auto maybeGround = locationquery::findGround(world, instanceLocation, true)) {
				SpawnEffectsAtLocation(
					emitter,
					nullptr,
					FVector { instanceLocation.X, instanceLocation.Y, maybeGround.GetValue() }
				);
			}

			angle += angleIncrement;
		}
	}

	SpawnEffectsAtLocation(nullptr, sound, location);
}

AEffectsActor* AEffectsActor::GetInstance(UWorld* world) {
	return actorquery::getFirstActor<AEffectsActor>(world);
}