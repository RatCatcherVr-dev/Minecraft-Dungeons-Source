// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/item/ProjectileGeneration.h"
#include "game/util/Pushback.h"
#include "NetherWartSporeGrenadeInstance.generated.h"


UCLASS()
class DUNGEONS_API USporeGrenadeGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	USporeGrenadeGameplayEffect();
};

UCLASS()
class DUNGEONS_API ASporeGrenade : public AActor {
	GENERATED_BODY()

public:
	ASporeGrenade();

	void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPreDestroy();

	float Damage;
protected:
	UPROPERTY(EditDefaultsOnly)
	float ExplosionRadius = 500.f;

	UPROPERTY(EditDefaultsOnly)
	FPushback Pushback;

	UPROPERTY(EditDefaultsOnly)
	float InitialCollisionDisableTime = 0.15f;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> InitialCollisionDisableTypes;

private:
	void EnableOverlaps();

	FTimerHandle EnableCollisionTimerHandle;
};

USTRUCT()
struct DUNGEONS_API FProjectileItemSpec {
	GENERATED_BODY()
public:
	FProjectileItemSpec();

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> ClassesToSpawn;

	UPROPERTY(EditDefaultsOnly)
	int AmountMin = 5;
	UPROPERTY(EditDefaultsOnly)
	int AmountMax = 5;

	UPROPERTY(EditDefaultsOnly)
	float VelocityMin = 1600.f;
	UPROPERTY(EditDefaultsOnly)
	float VelocityMax = 2600.f;

	UPROPERTY(EditDefaultsOnly)
	float ConeAngle = 12.f;

	UPROPERTY(EditDefaultsOnly)
	FVector LaunchDirectionFromOrigin;
};

UCLASS()
class DUNGEONS_API ANetherWartSporeGrenadeInstance : public AItemInstance {
	GENERATED_BODY()

public:
	ANetherWartSporeGrenadeInstance();
	void BeginPlay() override;
	void OnSetupWithValidOwner() override;
	void EndPlay(EEndPlayReason::Type Reason) override;

	float GetStats(EItemStats stat) const override;
protected:
	void Activate(const FPredictionKey& predictionKey) override;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnGrenades(FPredictionKey predictionKey, const TArray<FProjectileElement>& projectileElements);

	void SpawnGrenades(const TArray<FProjectileElement>& projectileElements);

	UPROPERTY(EditDefaultsOnly)
	FProjectileItemSpec ProjectileItemSpec;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 80.f;
private:
	class UReplicatedRandomSeedComponent* RandomComponent = nullptr;
	projectilegeneration::ProjectileGenerator ProjectileGen;
};
