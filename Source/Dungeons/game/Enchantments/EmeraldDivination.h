// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "EmeraldDivination.generated.h" 

UCLASS()
class DUNGEONS_API UEmeraldDivinationGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UEmeraldDivinationGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UEmeraldDivination : public UEnchantment
{
	UEmeraldDivination();

	GENERATED_BODY()

	void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int BlockThreshold = 1;

	/* 0 - 1 */
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float PercentageToSpawn = 0.02f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float EmeraldToSpawnBase = 1.f;
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float EmeraldToSpawnPerLevel = 2.f;
private:
	void OnBlocksExplored(int amount);
	void TrySpawn();
	void TryApplyEffect();
	void OnResetEffect();
	bool RandCheckCanTrigger();

	FTimerHandle ResetTimerHandle;
	FActiveGameplayEffectHandle EffectHandle;

	int blockCounter = 0;
public:
	FText CreateDescription() const override;

};
