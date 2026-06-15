#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "BackstabbersBrewInstance.generated.h"

UCLASS()
class DUNGEONS_API UBackstabbersBrewGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBackstabbersBrewGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName EffectDurationName;
};

UCLASS()
class DUNGEONS_API ABackstabbersBrewInstance : public AItemInstance
{
	GENERATED_BODY()
	
public:
	ABackstabbersBrewInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;

protected:
	UPROPERTY(EditDefaultsOnly, Transient, Category = "Dungeons")
	float MeleePowerBoostAmount = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)		
	TSubclassOf<UBackstabbersBrewGameplayEffect> Effect;
};
