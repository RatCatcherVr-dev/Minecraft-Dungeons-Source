// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/ArmorProperties/ArmorProperty.h"
#include "IncreasedMobTargeting.generated.h"


UCLASS()
class DUNGEONS_API UIncreasedMobTargeting : public UArmorProperty
{
	GENERATED_BODY()
	
public:
	UIncreasedMobTargeting();

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type reason) override;
};
