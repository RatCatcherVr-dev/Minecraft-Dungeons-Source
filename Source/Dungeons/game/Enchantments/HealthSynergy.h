// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Synergy.h"
#include "HealthSynergy.generated.h"


UCLASS()
class DUNGEONS_API UHealthSynergy : public USynergy
{
	GENERATED_BODY()
	
public:
	UHealthSynergy();
protected:
	void OnItemSuccess() override;
};
