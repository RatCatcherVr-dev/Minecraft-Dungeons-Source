// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffectTypes.h>
#include "game/component/RangedAttackComponent.h"
#include "MultiCharge.generated.h"

UCLASS()
class DUNGEONS_API UMultiChargeChargedEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMultiChargeChargedEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UMultiChargeChargedEffectOne : public UMultiChargeChargedEffect {
	GENERATED_BODY()
public:
	UMultiChargeChargedEffectOne(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMultiChargeChargedEffectTwo : public UMultiChargeChargedEffect {
	GENERATED_BODY()
public:
	UMultiChargeChargedEffectTwo(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMultiChargeChargedEffectThree : public UMultiChargeChargedEffect {
	GENERATED_BODY()
public:
	UMultiChargeChargedEffectThree(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMultiChargeChargingEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMultiChargeChargingEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMultiChargeDamageEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UMultiChargeDamageEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UMultiCharge : public UEnchantment
{
	GENERATED_BODY()
public:
	UMultiCharge();
	void OnStart() override;
	void OnEnd() override;
	void OnChargeComplete(float chargeTime);
	void OnMultiChargeComplete();
	void OnAfterRangedAttack(AActor* attackTarget, TSubclassOf<ABaseProjectile> ProjectileClass, bool charged, FVector spawnLocation, FRotator rotation, uint32 currentArrowCount, const FRandomStream& randStream, FPredictionKey key = FPredictionKey()) override;
	void OnAttackStopped();
private:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UMultiChargeChargedEffectOne> ChargedLevelOneEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UMultiChargeChargedEffectTwo> ChargedLevelTwoEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UMultiChargeChargedEffectThree> ChargedLevelThreeEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UMultiChargeChargingEffect> ChargingEffect;
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UMultiChargeDamageEffect> DamageEffect;

	UAbilitySystemComponent* AbilitySystem = nullptr;
	URangedAttackComponent* RangedComponent = nullptr;
	FActiveGameplayEffectHandle DamageEffectHandle;
	FDelegateHandle ChargeHandle;
	FDelegateHandle StopHandle;
	FTimerHandle ChargeTimer;
	float ChargeTime = 0.0f;
	int CurrentLevel = 0;
	FGameplayTag ChargeShootEffectTag;
	std::function<float(int)> EffectMultiplier;
	bool Active = false;
};
