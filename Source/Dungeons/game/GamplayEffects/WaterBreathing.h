#pragma once

#include "GameplayEffect.h"
#include "WaterBreathing.generated.h"

UCLASS()
class DUNGEONS_API UWaterBreathingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWaterBreathingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMobWaterBreathingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMobWaterBreathingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};