// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "IronHideAmuletInstance.generated.h"

UCLASS()
class DUNGEONS_API UDefenceGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDefenceGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API AIronHideAmuletInstance : public AItemInstance
{
	GENERATED_BODY()
	
public:
	AIronHideAmuletInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float DefenceBoostAmount = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float ApplyToAlliesRange = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FGameplayTag TriggerTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDefenceGameplayEffect> Effect;
};
