// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Explorer.generated.h" 

UCLASS()
class DUNGEONS_API UExplorerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UExplorerGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UExplorer : public UEnchantment
{
	UExplorer();

	GENERATED_BODY()
	void OnStart() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int BlockThreshold = 100;
private:
	void OnBlocksExplored(int amount);
	bool TryHeal();
	void TryApplyEffect();
	void OnResetEffect();

	FTimerHandle ResetTimerHandle;
	FActiveGameplayEffectHandle EffectHandle;

	int blockCounter = 0;
};
