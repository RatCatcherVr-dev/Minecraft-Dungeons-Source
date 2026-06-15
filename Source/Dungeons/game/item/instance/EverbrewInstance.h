// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include <GameplayEffectTypes.h>
#include "EverbrewInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API AEverbrewInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	AEverbrewInstance();

	virtual void ApplyEquippedEffects() override;
	virtual void RemoveEquippedEffects() override;

	int GetDisplayCount() const override;

	bool CanActivate() const override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float HealAmountPerSecond = 3.0f;	

	FActiveGameplayEffectHandle handle;
};
