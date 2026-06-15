#pragma once

#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "BurningBrewPotionInstance.generated.h"

UCLASS()
class DUNGEONS_API ABurningBrewPotionInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	ABurningBrewPotionInstance();

	int GetDisplayCount() const override;

	void Activate(const FPredictionKey& predictionKey) override;

	void CheckTargets();

	virtual void LifeSpanExpired() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UBurningBrewPotionGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Radius = 200;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobBurnDamage = 45.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BurnInterval = 0.5f;

private:
	UFUNCTION()
	void BurningBrewActive(bool isActive);
	UFUNCTION(Client, Unreliable)
	void Client_BurningBrewActive(bool isActive);
};

UCLASS()
class DUNGEONS_API UBurningBrewPotionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBurningBrewPotionGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UBurningBrewPotionDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBurningBrewPotionDamageGameplayEffect(const FObjectInitializer& ObjectInitializer);
};
