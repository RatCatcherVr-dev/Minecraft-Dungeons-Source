#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "BubbleColumnBurningEffect.generated.h"

UCLASS()
class DUNGEONS_API UBubbleColumnBurningEffect : public UGameplayEffect
{
	GENERATED_BODY()
public:
	UBubbleColumnBurningEffect();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float DamagePercentPerSecond = 0.2f;
};