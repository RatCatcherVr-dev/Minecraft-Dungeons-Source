// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "GameplayEffect.h"
#include "game/util/ValueFormat.h"
#include "../Enchantments/Blind.h"
#include "SquidRoll.generated.h"


UCLASS()
class DUNGEONS_API USquidRollQuick : public UArmorProperty
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	USquidRollQuick();

	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABlindCloud> BlindCloudClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBlindGameplayEffect> BlindGameplayEffect;

	UFUNCTION()
	void OnCloudDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Duration = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int MaxCloudsAtSameTime = 3;

	FActiveGameplayEffectHandle Handle;
	int CurrentActiveClouds = 0;
};

UCLASS()
class DUNGEONS_API USquidRollLimited : public UArmorProperty
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;

	USquidRollLimited();

	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ABlindCloud> BlindCloudClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBlindGameplayEffect> BlindGameplayEffect;

	UFUNCTION()
	void OnCloudDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Duration = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int MaxCloudsAtSameTime = 3;

	FActiveGameplayEffectHandle Handle;
	int CurrentActiveClouds = 0;
};
