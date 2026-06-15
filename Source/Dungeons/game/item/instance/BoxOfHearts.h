// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include <WeakObjectPtrTemplates.h>
#include "BoxOfHearts.generated.h"

UCLASS()
class DUNGEONS_API UBoxOfHeartsGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBoxOfHeartsGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API ABoxOfHearts : public AItemInstance
{
	GENERATED_BODY()
public:
	ABoxOfHearts();

	void Activate(const FPredictionKey& predictionKey) override;

	void EndPlay(EEndPlayReason::Type reason) override;

	float GetStats(EItemStats stat) const override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

protected:
	void OnKillMob(TWeakObjectPtr<class AMobCharacter> mob);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBoxOfHeartsGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CharmedDamagePercentageIncreasePerDisplayItemPower = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CharmedTakeDamageMultiplier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CharmedSpeedMultiplier = 1.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int NumCharmedMobs = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CharmedMobDuration = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CharmRange = 1000.0f;

	UPROPERTY(Replicated)
	TArray<TWeakObjectPtr<class AMobCharacter>> BefriendedMobs;
};
