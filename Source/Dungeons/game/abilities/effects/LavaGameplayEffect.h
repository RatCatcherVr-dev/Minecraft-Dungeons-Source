#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "LavaGameplayEffect.generated.h"

UCLASS()
class DUNGEONS_API ULavaGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	ULavaGameplayEffect();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float DamagePerSecond = 180.f;
};
