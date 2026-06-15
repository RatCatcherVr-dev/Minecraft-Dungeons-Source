// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/IntervalExecutionEnchantment.h"
#include "game/actor/ShulkerBullet.h"
#include "ShulkerSentry.generated.h"


UCLASS(BlueprintType)
class DUNGEONS_API AFriendlyShulkerBullet : public AShulkerBullet {
	GENERATED_BODY()
public:
	AFriendlyShulkerBullet();

	void Tick(float DeltaTime) override;
	
	void OnHitboxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

private:

};

UCLASS(BlueprintType)
class DUNGEONS_API AShulkerBulletShooter : public AActor {
	GENERATED_BODY()
public:
	AShulkerBulletShooter();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	void FireBulletAtTarget(AActor* target);

	void SetCenterCharacter(ABaseCharacter* character);

	TWeakObjectPtr<ABaseCharacter> GetCenterCharacter() const;
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float CenterOffset = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float CirculationSpeedScale = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	TSubclassOf<AFriendlyShulkerBullet> FriendlyShulkerBulletClass;
private:
	UPROPERTY(Transient, Replicated)
	TWeakObjectPtr<ABaseCharacter> CenterCharacter;
};

UCLASS()
class DUNGEONS_API UShulkerSentry : public UIntervalExecutionEnchantment {
	GENERATED_BODY()
public:
	UShulkerSentry();
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
	TSubclassOf<AShulkerBulletShooter> ShulkerBulletShooterClass;
private:
	UPROPERTY(Transient, Replicated)
	TWeakObjectPtr<AShulkerBulletShooter> ShulkerBulletShooter;
};
