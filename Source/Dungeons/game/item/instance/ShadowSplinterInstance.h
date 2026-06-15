// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "ShadowSplinterInstance.generated.h"

UCLASS()
class DUNGEONS_API AShadowSplinterInstance : public AItemInstance
{
	GENERATED_BODY()

	AShadowSplinterInstance();

	int GetDisplayCount() const override;
	void Activate(const FPredictionKey& predictionKey) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TSubclassOf<UShadowSplinterGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, Transient, Category = "Dungeons")
		float MeleePowerBoostAmount = 8.0f;

	UFUNCTION()
	void OnInvisibilityChanged(const FGameplayTag tag, const int32 tagCount);

	void TriggerCooldown();

private:
	FTimerHandle CooldownTimerHandler;
	FPredictionKey PredictionKey;
	UAbilitySystemComponent* OwnerAbilitySystem;
	FActiveGameplayEffectHandle Handle;

public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	bool CanActivate() const override;


	float GetSoulActivationCost() const override;
	void RemoveEquippedEffects() override;
};

UCLASS()
class DUNGEONS_API UShadowSplinterGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UShadowSplinterGameplayEffect(const FObjectInitializer& ObjectInitializer);
};
