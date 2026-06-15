// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/TotemOfShielding.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "TotemOfShielding_Unique1.generated.h"

class ABaseProjectile;

UCLASS()
class DUNGEONS_API UTotemOfShielding_Unique1DamageGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UTotemOfShielding_Unique1DamageGameplayEffect();
};


UCLASS()
class DUNGEONS_API ATotemOfShieldingActor_Unique1 : public ATotemOfShielding
{
	GENERATED_BODY()
public:
	float Power = 1.0f;
protected:
	void OnDestroyCountdownStarted_Internal() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ExplosionDamage = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float ExplosionRadius = 800.0f;
};

UCLASS()
class DUNGEONS_API ATotemOfShieldingInstance_Unique1 : public ATotemOfShieldingInstance {
	GENERATED_BODY()
};
