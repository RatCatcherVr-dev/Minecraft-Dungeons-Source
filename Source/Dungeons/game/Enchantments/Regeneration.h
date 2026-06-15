// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayModMagnitudeCalculation.h>
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "Regeneration.generated.h"

 
UCLASS()
class DUNGEONS_API URegenerationGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URegenerationGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UHealthPerSecondCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:

	static const FName HealthPerSecondName;
	UHealthPerSecondCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};


UCLASS()
class DUNGEONS_API URegeneration : public UEnchantment
{
	GENERATED_BODY()
public:
	URegeneration();

	FText CreateDescription() const override;

	void OnStart() override;

	void OnEnd() override;

	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;
	
	void OnStartRegeneration();

	void ResetTimer();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<URegenerationGameplayEffect> Effect;
private:
	FTimerHandle timerHandle;

	FActiveGameplayEffectHandle effectHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobRegenerationAmountPerSecond = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float RegenerationAmountPerSecond = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobAmountPerSecond = 3.0f;
		
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobTimeUntilRegeneration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float TimeUntilRegeneration = 10.0f;	

	void StopRegeneration();

	bool bIsPlayerOwner = true;
};
