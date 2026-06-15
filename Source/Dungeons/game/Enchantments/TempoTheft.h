// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "Freezing.h"
#include "TempoTheft.generated.h"

class AMobCharacter;

UCLASS()
class DUNGEONS_API UTempoTheftBoostEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UTempoTheftBoostEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UTempoTheftGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UTempoTheftGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API ASpeedBoostOnOverlapActor : public AActor {
	GENERATED_BODY()
public:
	ASpeedBoostOnOverlapActor();

	TWeakObjectPtr<ABaseCharacter> TargetCharacter;
	float SpeedBoostAmount;
	float SpeedBoostDuration;
	float Level;

	void BeginPlay() override;
	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(BlueprintNativeEvent)
	void OnGrantSpeed(ABaseCharacter* character);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UTempoTheftBoostEffect> OwnerEffect;
private:
	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Overlap;
	UPROPERTY(VisibleAnywhere)
	class UMoveToTargetMovementComponent* Movement;
};

/**
*
*/
UCLASS()
class DUNGEONS_API UTempoTheft : public UEnchantment
{
	GENERATED_BODY()

	UTempoTheft();

	FText CreateDescription() const override;

	void OnAfterDealtRangedDamage(float Damage, ABaseProjectile* fromProjectile, AActor* toWhat, AActor* toWhom, FVector fromLocation, FVector atLocation, FRandomStream& randStream) override;

	void OnStealSpeed(class ABaseCharacter* toStealFrom, FVector atLocation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnSoul(ABaseCharacter* owner, const FVector& spawnLocation, float speedBoostAmount, float speedBoostDuration);
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UTempoTheftGameplayEffect> TargetEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<ASpeedBoostOnOverlapActor> ActorClass;

	UPROPERTY(EditDefaultsOnly)
	float AmountToStealPerLevel = 0.1666f;

	UPROPERTY(EditDefaultsOnly)
	float MobAmountToSteal = 0.5f;

	float StealTime = 4.f;

	float MobStealTime = 6.f;
};
