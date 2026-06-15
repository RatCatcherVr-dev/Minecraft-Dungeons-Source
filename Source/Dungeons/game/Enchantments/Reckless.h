// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "EffectApplyingEnchantment.h"
#include "Reckless.generated.h"


UCLASS()
class DUNGEONS_API URecklessGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URecklessGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName HealthReductionKey;
	static const FName DamageIncreaseKey;
};


/**
 * 
 */
UCLASS()
class DUNGEONS_API UReckless : public UEffectApplyingEnchantment
{
	GENERATED_BODY()
public:
	UReckless();

	void OnPreSpecApplication(FGameplayEffectSpec& mutableSpec) override;

	FText CreateDescription() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DamageIncreaseAtLevelOne = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DamageIncreasePerLevel = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float HealthDecrease = 0.6f;

	float GetHealthReductionAsMultipierClamped() const;
};
