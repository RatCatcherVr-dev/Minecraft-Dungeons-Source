#pragma once

#include "AItemInstance.h"
#include "HealthPotionInstance.generated.h"

UCLASS()
class DUNGEONS_API UHealthPotionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UHealthPotionGameplayEffect();
};

UCLASS()
class DUNGEONS_API AHealthPotionInstance : public AItemInstance {
	GENERATED_BODY()
public:
	bool CanActivate() const override;

	void Activate(const FPredictionKey& predictionKey) override;
};
