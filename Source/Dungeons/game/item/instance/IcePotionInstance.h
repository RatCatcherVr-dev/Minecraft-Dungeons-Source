#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "IcePotionInstance.generated.h"


UCLASS()
class DUNGEONS_API UIcePotionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UIcePotionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API AIcePotionInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	AIcePotionInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float DefenseBoost = 1.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UIcePotionGameplayEffect> Effect;
};
