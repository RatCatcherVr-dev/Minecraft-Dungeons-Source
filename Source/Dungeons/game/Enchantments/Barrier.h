// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include <GameplayModMagnitudeCalculation.h>
#include "Barrier.generated.h"

UCLASS() 
class DUNGEONS_API UBarrierModLevelCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class DUNGEONS_API UBarrierGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBarrierGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UBarrier : public UEnchantment
{
	GENERATED_BODY()
public:
	UBarrier();

	void OnStart() override;

	void OnEnd() override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float AffectionRadius = 1500.0f;
private:
	UFUNCTION()
	void OnRadiusEnter(UPrimitiveComponent* ThisComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnRadiusLeave(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	USphereComponent *radiusSphere;

	TSet<TWeakObjectPtr<ABaseCharacter>> SeenTargets;
};
