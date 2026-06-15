// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/OnLandingExecutionEnchantment.h"
#include "Swiftfooted.generated.h"

UCLASS()
class DUNGEONS_API USwiftfootedGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USwiftfootedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API USwiftfooted : public UEnchantment
{
	GENERATED_BODY()
public:
	USwiftfooted();

	FText CreateDescription() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float swiftDuration = 3.0f;

	void OnDodgeRollEnd(FPredictionKey) override;

private:
	void ApplyEffect(FPredictionKey);

	FTimerHandle TimerHandle;
};
