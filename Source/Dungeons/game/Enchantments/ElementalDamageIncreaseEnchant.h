// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "EffectApplyingEnchantment.h"
#include "ElementalDamageIncreaseEnchant.generated.h"


UCLASS()
class DUNGEONS_API UElementalDamageIncreaseGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UElementalDamageIncreaseGameplayEffect(const FObjectInitializer& ObjectInitializer);

	static const FName DamageKey;
};

UCLASS()
class DUNGEONS_API ULightningTouchedGameplayEffect : public UElementalDamageIncreaseGameplayEffect {
	GENERATED_BODY()
public:
	ULightningTouchedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UFireTouchedGameplayEffect : public UElementalDamageIncreaseGameplayEffect {
	GENERATED_BODY()
public:
	UFireTouchedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API USoulTouchedGameplayEffect : public UElementalDamageIncreaseGameplayEffect {
	GENERATED_BODY()
public:
	USoulTouchedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UPoisonTouchedGameplayEffect : public UElementalDamageIncreaseGameplayEffect {
	GENERATED_BODY()
public:
	UPoisonTouchedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


/**
 * 
 */
UCLASS()
class DUNGEONS_API UElementalDamageIncreaseEnchant : public UEffectApplyingEnchantment
{
	GENERATED_BODY()
public:
	UElementalDamageIncreaseEnchant();

	void OnPreSpecApplication(FGameplayEffectSpec& mutableSpec) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DamageIncreasePerLevel = 1.f;
};
