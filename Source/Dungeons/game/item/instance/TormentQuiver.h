// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/ArrowCraftingItemInstance.h"
#include "game/actor/item/Arrow.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "TormentQuiver.generated.h"

UCLASS()
class DUNGEONS_API ATormentProjectile : public AArrow {
	GENERATED_BODY()
public:
	ATormentProjectile(const FObjectInitializer& ObjectInitializer);
	void BeginPlay() override;

	void Tick(float DeltaSecs) override;

	virtual void ResetProjectile() override;
	virtual void LaunchProjectile(AActor* pOwner) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float LifetimeThresholdSeconds = 1.0f;

	UFUNCTION(BlueprintImplementableEvent)
	void OnLifetimeThresholdReached();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dungeons")
	float RotationSpeed = 0.0f;

private:
	bool bLifeTimeThresholdReached;

	UPROPERTY()
	class USceneComponent* RotationPivot = nullptr;

	UPROPERTY()
	class UBoxComponent* CollisonBox = nullptr;

};

UCLASS()
class DUNGEONS_API UTormentProjectileGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	static const float BASE_HIT_DAMAGE;
	UTormentProjectileGameplayEffect();

};

UCLASS()
class DUNGEONS_API ATormentQuiver : public AArrowCraftingItemInstance {
	GENERATED_BODY()
public:
	ATormentQuiver();
	float GetStats(EItemStats stat) const override;
private:
	TWeakObjectPtr<USoulComponent> OwnerSoulComponent;
};

