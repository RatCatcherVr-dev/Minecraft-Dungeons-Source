#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameplayModMagnitudeCalculation.h"
#include "StaggerGamePlayEffect.generated.h"



UCLASS()
class DUNGEONS_API UStaggerGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UStaggerGameplayEffect();
};


UCLASS()
class DUNGEONS_API UEnduranceRecoveryGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UEnduranceRecoveryGameplayEffect();
};

UCLASS()
class DUNGEONS_API UStunImmunityGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UStunImmunityGameplayEffect();
};


UCLASS()
class DUNGEONS_API UTemporaryStunimmunityGameplayEffect : public UGameplayEffect{
	GENERATED_BODY()
public:
	UTemporaryStunimmunityGameplayEffect();
};