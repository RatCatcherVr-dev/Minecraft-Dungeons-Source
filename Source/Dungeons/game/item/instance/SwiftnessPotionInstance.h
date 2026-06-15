#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "SwiftnessPotionInstance.generated.h"

UCLASS()
class DUNGEONS_API USwiftnessPotionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USwiftnessPotionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API ASwiftnessPotionInstance : public AItemInstance
{
	GENERATED_BODY()

	ASwiftnessPotionInstance();

	void Activate(const FPredictionKey& predictionKey) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float SpeedBoostAmount = 1.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<USwiftnessPotionGameplayEffect> Effect;
};
