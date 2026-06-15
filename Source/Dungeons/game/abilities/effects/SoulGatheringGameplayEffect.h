#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SoulGatheringGameplayEffect.Generated.h"

UCLASS()
class DUNGEONS_API USoulGatheringGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USoulGatheringGameplayEffect();

	static const FName SoulGatheringMagnitude;
};