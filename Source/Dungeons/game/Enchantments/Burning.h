// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/IntervalExecutionEnchantment.h"
#include "Burning.generated.h"

UCLASS()
class DUNGEONS_API UBurningDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBurningDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API UBurning : public UIntervalExecutionEnchantment
{
	GENERATED_BODY()
	
public:
	UBurning();

	FText CreateDescription() const override;

protected:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type endPlayReason) override;

	void Execution() override;

	float GetExecutionInterval() const override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AActor> FireAuraActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TWeakObjectPtr<UChildActorComponent> FireAuraChildActor;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float Radius = 300.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BurnBaseDamage = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float MobBurnDamage = 45.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float BurnInterval = 0.5f;
private:
	UFUNCTION()
	void OnOwnerDeath();

	void SpawnFireAura();

	void RemoveFireAura();

	void OnBurn();
};
