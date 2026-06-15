#pragma once

#include "GameplayEffect.h"
#include "ConduitGameplayEffects.generated.h"

UCLASS()
class DUNGEONS_API UMobDrowningGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMobDrowningGameplayEffect(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float PercentageDamagePerSecond = 10.0f;
};

UCLASS()
class DUNGEONS_API UConduitSpeedBoostGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UConduitSpeedBoostGameplayEffect(const FObjectInitializer& ObjectInitializer);

	// modify this to change the speed boost amount.
	// 1.1f = 10% speed boost, 1.2f = 20% etc.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float SpeedMultiplier = 1.2f;
};