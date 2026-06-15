// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"

#include "game/item/instance/AItemInstance.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"

#include "CorruptedSeeds.generated.h"

UCLASS()
class DUNGEONS_API UCorruptedSeedsEntangleEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCorruptedSeedsEntangleEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UCorruptedSeedsPoisonEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UCorruptedSeedsPoisonEffect();
};

UCLASS()
class DUNGEONS_API ACorruptedSeeds : public AItemInstance
{
	GENERATED_BODY()

public:
	ACorruptedSeeds();
	void Activate(const FPredictionKey& predictionKey) override;

	float GetStats(EItemStats stat) const override;

protected:
	void PopulateBulletPoints(TArray<FItemBulletPoint>&, const ABaseCharacter& owner) const override;

	TArray<class AMobCharacter*> GetMobsInRange();

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Radius = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int TargetCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float PoisonDamagePeriod = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float PoisonDamageScale = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	FGameplayTag TriggerTag;

	UPROPERTY()
	TSubclassOf<UCorruptedSeedsEntangleEffect> EntangleEffect;

	UPROPERTY()
	TSubclassOf<UCorruptedSeedsPoisonEffect> PoisonEffect;
};
