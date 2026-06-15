// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "OnLandingExecutionEnchantment.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UOnLandingExecutionEnchantment : public UEnchantment
{
	GENERATED_BODY()
public:
	UOnLandingExecutionEnchantment();

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
protected:
	virtual void OnLanded() {}
private:
	bool bPlayerWasInAir = false;
};
