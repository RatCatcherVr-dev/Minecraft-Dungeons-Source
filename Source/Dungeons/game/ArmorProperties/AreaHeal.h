// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "AreaHeal.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UAreaHeal : public UArmorProperty
{
	GENERATED_BODY()
public:
	UAreaHeal();

	void BeginPlay();

	UFUNCTION()
	void OnHealthPotionActivated(UItemSlot* slot, bool success);

protected:
	UPROPERTY(EditDefaultsOnly)
	float Radius = 300.0f;
};
