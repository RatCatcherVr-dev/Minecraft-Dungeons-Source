// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "game/GameSettings.h"
#include "game/item/drop/ItemDropGenerator.h"
#include "game/item/ItemRarity.h"
#include "LuckOfTheSea.generated.h"


UCLASS()
class DUNGEONS_API ULuckOfTheSea : public UEnchantment
{
	GENERATED_BODY()
	
	ULuckOfTheSea();
protected:
	UPROPERTY(EditAnywhere, Category = "Luck Levels")
	float luck_level_1 = .1f;
	UPROPERTY(EditAnywhere, Category = "Luck Levels")
	float luck_level_2 = .2f;
	UPROPERTY(EditAnywhere, Category = "Luck Levels")
	float luck_level_3 = .3f;

public:
	void OnStart() override;
	void OnEnd() override;

	void OnLuckChanged(FPredictionKey) override;


	FText CreateDescription() const override;


	TOptional<UGearUtil*> ChangeItemRarity(EItemRarity& rarity) override;

private:
	FActiveGameplayEffectHandle Handle;
};