// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "calculations/DifficultyModCalculation.h"
#include "DifficultyGameplayEffect.generated.h"

/**
 *
 */
UCLASS()
class DUNGEONS_API UBaseDifficultyGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UBaseDifficultyGameplayEffect();
};

UCLASS()
class DUNGEONS_API UNormalDifficultyGameplayEffect : public UBaseDifficultyGameplayEffect
{
	GENERATED_BODY()
public:
	UNormalDifficultyGameplayEffect();
};


UCLASS()
class DUNGEONS_API USpecialDifficultyGameplayEffect : public UBaseDifficultyGameplayEffect
{
	GENERATED_BODY()
public:
	USpecialDifficultyGameplayEffect();
};

UCLASS()
class DUNGEONS_API UPlayerDifficultyGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UPlayerDifficultyGameplayEffect();
};