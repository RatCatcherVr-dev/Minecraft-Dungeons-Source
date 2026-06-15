// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "LavaBurningGamePlayEffect.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API ULavaBurningGamePlayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	ULavaBurningGamePlayEffect();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float DamagePerSecond = 15.f;
};
