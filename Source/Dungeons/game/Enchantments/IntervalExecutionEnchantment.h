// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "IntervalExecutionEnchantment.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UIntervalExecutionEnchantment : public UEnchantment
{
	GENERATED_BODY()
public:
	UIntervalExecutionEnchantment();
protected:
	void OnStart() override;
	void OnEnd() override;

	virtual void Execution() {}

	virtual bool ShouldIntervalExecute() const;

	virtual void IntervalExecutionStarted() {}
	virtual void IntervalExecutionStopped() {}
	virtual float GetExecutionInterval() const;

	bool bHasStarted = true;
private:
	UFUNCTION()
	void OnIntervalExecute();

	void ResetTimer();

	FTimerHandle TimerHandle;
};
