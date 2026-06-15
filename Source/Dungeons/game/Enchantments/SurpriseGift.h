// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/StorableDropBaseEnchantment.h"
#include "SurpriseGift.generated.h"



UCLASS()
class DUNGEONS_API USurpriseGift : public UStorableDropBaseEnchantment {
	GENERATED_BODY()
private:
	USurpriseGift();
protected:
	int GetDropAmount() const override;
};
