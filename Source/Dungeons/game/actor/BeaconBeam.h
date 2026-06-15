#pragma once

#include "character/player/PlayerCharacter.h"
#include "BeaconBeam.generated.h"

UCLASS()
class DUNGEONS_API ABeaconBeam : public AActor {
	GENERATED_BODY()

public:
	ABeaconBeam(const FObjectInitializer& objectInitializer);

	void BeginPlay() override;

	void Tick(float deltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
	float beamRadius = 100.f;

	UPROPERTY(EditDefaultsOnly)
	float delay = .5f;

	float startTime;

	UPROPERTY(EditDefaultsOnly)
	float beamLengthMax = 3000.f;

	UPROPERTY(EditDefaultsOnly)
	float damageAmount = 50.f;

	UPROPERTY(EditDefaultsOnly)
	float damageInterval = 250.f;

	UPROPERTY(EditDefaultsOnly)
	int emitterIndex = 0;

	UPROPERTY(EditDefaultsOnly)
	int targetIndex = 0;

	TArray<UParticleSystemComponent*> beams;

	float damageTime;

	UPROPERTY(EditDefaultsOnly)
	float sweepSpan = 1.f;

	UPROPERTY(EditDefaultsOnly)
	float sweepTime = 2.f;
	///0 = infinite
	UPROPERTY(EditDefaultsOnly)
	float damageDuration = 0.f;

	float initialYaw;

	float age;

	void TickBeam(FVector& beamStartLocation, float& beamLength);

	void TickDamage(FVector beamStartLocation, float beamLength);
};

UCLASS()
class DUNGEONS_API UArchBeamDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UArchBeamDamageGameplayEffect();
};