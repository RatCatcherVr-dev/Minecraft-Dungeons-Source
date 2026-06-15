// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/TotemBaseItemInstance.h"
#include "TotemOfShielding.generated.h"


UCLASS()
class DUNGEONS_API ATotemOfShielding : public ATotemBaseActor
{
	GENERATED_BODY()
public:
	ATotemOfShielding();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	float Power;

	UPROPERTY(Replicated)
	float Duration;
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void OnDestroyCountdownStarted_Internal() override;

	float GetHealthFraction() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnProjectileImpact(ABaseProjectile* projectile, const FVector& impactLocation);

	virtual void OnProjectileImpact_Internal(ABaseProjectile* projectile) {}

private:
	FTimerHandle DestructionTimerHandle;

	bool CanBeDamagedBy(ABaseProjectile* projectile);

	UFUNCTION()
	void OnBeginTorusOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};


UCLASS()
class DUNGEONS_API ATotemOfShieldingInstance : public ATotemBaseItemInstance {
	GENERATED_BODY()

public:
	ATotemOfShieldingInstance();

protected:
	void PreTotemBeginPlay(ATotemBaseActor* totemActor) override;
};
