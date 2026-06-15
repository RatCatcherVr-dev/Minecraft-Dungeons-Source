#pragma once

#include <GameplayEffect.h>
#include "ItemPowerGameplayEffects.generated.h"

UCLASS()
class UMeleeDamageItemPowerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMeleeDamageItemPowerGameplayEffect();
};

UCLASS()
class UMeleeHealingItemPowerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMeleeHealingItemPowerGameplayEffect();
};

UCLASS()
class URangedDamageItemPowerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URangedDamageItemPowerGameplayEffect();
};

UCLASS()
class URangedHealingItemPowerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	URangedHealingItemPowerGameplayEffect();
};


UCLASS()
class UMaxHealthItemPowerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMaxHealthItemPowerGameplayEffect();
};


UCLASS()
class UArmorDamageItemPowerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UArmorDamageItemPowerGameplayEffect();
};


UCLASS()
class UArmorHealingItemPowerGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UArmorHealingItemPowerGameplayEffect();
};