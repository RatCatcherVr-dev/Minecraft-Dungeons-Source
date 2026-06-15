// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/IntervalExecutionEnchantment.h"
#include "Electrified.generated.h"

 

UCLASS()
class DUNGEONS_API UElectrified : public UIntervalExecutionEnchantment
{
	GENERATED_BODY()
public:
	FText CreateDescription() const override;

	UElectrified();
protected:

	UPROPERTY(EditDefaultsOnly)
	FPushback OnKillPushback;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float ZapDelayMob = 4.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int ZapCountMob = 1;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	int ZapCount = 2;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float ZapDamage = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float MobZapDamage = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float Radius = 700.0f;


	void Execution() override;

	bool ShouldTriggerOnRoll() const;

	float GetExecutionInterval() const override;

	void OnDodgeRollStart(const FVector& dodgeDirection, FPredictionKey) override;

	void OnDodgeRollEnd(FPredictionKey) override;

	bool ShouldIntervalExecute() const override;

	void IntervalExecutionStarted() override;
	void IntervalExecutionStopped() override;

	void StartElectrifiedEffect(FPredictionKey key = FPredictionKey());
	void StopElectrifiedEffect(FPredictionKey key = FPredictionKey());
private:
	void OnZap(FPredictionKey key = FPredictionKey());
};

UCLASS()
class DUNGEONS_API UElectrifiedDamageGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UElectrifiedDamageGameplayEffect();
};
