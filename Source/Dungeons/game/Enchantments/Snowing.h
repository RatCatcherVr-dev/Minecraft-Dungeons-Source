// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/IntervalExecutionEnchantment.h"
#include "Snowing.generated.h"


UCLASS(BlueprintType)
class DUNGEONS_API ASnowBall : public AActor { 
	GENERATED_BODY()
public:
	ASnowBall();

	void BeginPlay() override;

	void SetThrowerOwner(class ASnowBallThrower* owner);

	void SetStunDuration(float stunDuration);

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TSubclassOf<class UStaggerGameplayEffect> Effect;
private:
	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	float StunDuration;

	class UProjectileMovementComponent* Movement;

	TWeakObjectPtr<ASnowBallThrower> ThrowerOwner;
};

UCLASS(BlueprintType)
class DUNGEONS_API ASnowBallThrower : public AActor {
	GENERATED_BODY()
public:
	ASnowBallThrower();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void ShootSnowBallAtTarget(AActor* target, float SnowBallStunDuration);

	void SetCenterCharacter(ABaseCharacter* character);

	TWeakObjectPtr<ABaseCharacter> GetCenterCharacter() const;
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float CenterOffset = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float CirculationSpeedScale = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TSubclassOf<ASnowBall> SnowBallClass;
private:
	UPROPERTY(Transient, Replicated)
	TWeakObjectPtr<ABaseCharacter> CenterCharacter;
};

UCLASS()
class DUNGEONS_API USnowing : public UIntervalExecutionEnchantment {
	GENERATED_BODY()
public:
	USnowing();
protected:
	void OnStart() override;
	void OnEnd() override;

	void Execution() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	float GetExecutionInterval() const override;
	
	UPROPERTY(EditAnywhere)
	float MobStunDuration = 2.0f;

	UPROPERTY(EditAnywhere)
	float PlayerStunDuration = 1.0f;

	UPROPERTY(EditAnywhere)
	float Radius = 1000.f;

	UPROPERTY(EditAnywhere)
	float BaseInterval = 5.f;
	
	UPROPERTY(EditAnywhere)
	float IntervalPerLevel = -2.f;

	UPROPERTY(EditAnywhere)
	float MinInterval = 1.f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TSubclassOf<ASnowBallThrower> SnowBallThrowerClass;
private:
	UPROPERTY(Transient, Replicated)
	TWeakObjectPtr<ASnowBallThrower> SnowballThrower;
};
