// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/MobSummonItem.h"
#include "LostEvocation.generated.h"

UCLASS()
class DUNGEONS_API ALostEvocation : public AMobSummonItem {
	GENERATED_BODY()
public:
	ALostEvocation();
protected:
	bool WillSummon() const override;
};