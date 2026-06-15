// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "game/component/MeleeAttackComponent.h"
#include "Shockwave.generated.h"


UCLASS()
class DUNGEONS_API AShockWaveProjectile : public AActor {
	GENERATED_BODY()
public:
	AShockWaveProjectile();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	FVector MoveVector;
	
	float Damage;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Duration = 0.4f;
private:
	void NotifyActorBeginOverlap(AActor* OtherActor) override;
	void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
};

UCLASS()
class DUNGEONS_API UShockwave : public UEnchantment
{
	GENERATED_BODY()
public:
	UShockwave();
protected:

	void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AShockWaveProjectile> ShockWaveProjectileClass;
private:
	void SpawnShockwave(FVector direction);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnShockwave(FVector direction, FPredictionKey key);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseDamage = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BaseMoveSpeed = 1850.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float spawnOffset = 150.0;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float spawnSafetyMargin = 15.0;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobDamage = 125.0f;

	TWeakObjectPtr<AShockWaveProjectile> SpawnedShockWave;
};

UCLASS()
class DUNGEONS_API UShockwaveDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UShockwaveDamageGameplayEffect();
};