#pragma once

#include <CoreMinimal.h>
#include <GameplayEffect.h>
#include "ToxicWaterDamageGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API UToxicWaterDamageGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UToxicWaterDamageGameplayEffect();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float DamagePerSecond = 120.f;
};
