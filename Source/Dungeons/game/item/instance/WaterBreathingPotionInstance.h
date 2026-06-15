#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "WaterBreathingPotionInstance.generated.h"

UCLASS()
class DUNGEONS_API AWaterBreathingPotionInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	AWaterBreathingPotionInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UWaterBreathingPotionGameplayEffect> Effect;
};

UCLASS()
class DUNGEONS_API UWaterBreathingPotionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UWaterBreathingPotionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};