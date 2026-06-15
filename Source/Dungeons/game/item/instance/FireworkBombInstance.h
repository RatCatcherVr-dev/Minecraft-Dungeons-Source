// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "NetherWartSporeGrenadeInstance.h"
#include "FireworkBombInstance.generated.h"

UCLASS()
class DUNGEONS_API UFireworkBombGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UFireworkBombGameplayEffect();
};

UCLASS()
class DUNGEONS_API AFireworkBombProjectile : public AActor {
	GENERATED_BODY()
public:
	AFireworkBombProjectile();

	void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	float Damage;
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnPreDestroy();

	UPROPERTY(EditDefaultsOnly)
	float ExplosionRadius = 500.f;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ExplosionTag;
};


UCLASS()
class DUNGEONS_API AFireworkBombActor : public AActor {
public:
	GENERATED_BODY()
	AFireworkBombActor();

	void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	TArray<FProjectileElement> ProjectileElements;

	float BombDamage;
	float ProjectileDamage;
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnPreDestroy();

	UPROPERTY(EditDefaultsOnly)
	float ExplosionRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ExplosionTag;
private:
	void SetProjectilesTransform();
};


/**
 *
 */
UCLASS()
class DUNGEONS_API AFireworkBombInstance : public AItemInstance {
	GENERATED_BODY()
public:
	AFireworkBombInstance();

	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type Reason) override;

	void Activate(const FPredictionKey& predictionKey);

	float GetStats(EItemStats stat) const override;
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFireworkBombActor> BombActorClass;

	UPROPERTY(EditDefaultsOnly)
	float BombLaunchSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly)
	FProjectileItemSpec ProjectileItemSpec;

	UPROPERTY(EditDefaultsOnly)
	float BombDamage = 50.f;

	UPROPERTY(EditDefaultsOnly)
	float ProjectileDamage = 50.f;
private:
	void SpawnBomb(const FVector& launchDir, const TArray<FProjectileElement>& projectileElements);

	UFUNCTION(NetMulticast, UnReliable)
	void MulticastSpawnBomb(FPredictionKey predictionKey, const FVector& launchDir, const TArray<FProjectileElement>& projectileElements);

	TWeakObjectPtr<AFireworkBombActor> Bomb;

	projectilegeneration::ProjectileGenerator ProjGenerator;

	class UReplicatedRandomSeedComponent* RandomComponent = nullptr;
};