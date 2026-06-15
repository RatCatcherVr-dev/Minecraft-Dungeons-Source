#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "game/ArmorProperties/Heavyweight.h"
#include "DenseBrewPotionInstance.generated.h"


UCLASS()
class DUNGEONS_API UDenseBrewPotionGameplayEffect : public UHeavyweightGameplayEffect {
	GENERATED_BODY()
public:
	UDenseBrewPotionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API ADenseBrewInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	ADenseBrewInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float PushbackResistance = 0.75f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	float MeleeDamageResistance = 0.75f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UDenseBrewPotionGameplayEffect> Effect;
};
