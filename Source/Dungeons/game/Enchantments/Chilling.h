// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/IntervalExecutionEnchantment.h"
#include "game/Enchantments/Freezing.h"
#include "Chilling.generated.h"

UCLASS()
class UChillingGameplayEffect : public UFreezingGameplayEffect {
	GENERATED_BODY()
public:
	UChillingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UChilling : public UIntervalExecutionEnchantment
{
	GENERATED_BODY()
public:
	UChilling();

	FText CreateDescription() const override;

protected:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type endPlayReason) override;

	void Execution() override;

	float GetExecutionInterval() const override;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float FreezeTime = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float FreezeInterval = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float Radius = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AActor> ChillingAuraActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TWeakObjectPtr<UChildActorComponent> ChillingAuraChildActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UChillingGameplayEffect> TargetEffect;
private:
	UFUNCTION()
	void OnOwnerDeath();

	void OnBlast();

	void SpawnChillingAura();
	void RemoveChillingAura();
};
