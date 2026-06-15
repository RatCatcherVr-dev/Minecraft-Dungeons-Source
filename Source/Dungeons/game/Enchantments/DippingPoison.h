// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "DippingPoison.generated.h"

UCLASS()
class DUNGEONS_API UDippingPoison : public UEnchantment
{
	GENERATED_BODY()
public:
	UDippingPoison();
	void OnStart() override;
	void OnEnd() override;


	void OnItemActivated(const AItemInstance*, FPredictionKey key) override;

private:
	UPROPERTY(EditAnywhere, Category = "Luck Levels")
		int arrows_given_1 = 5;
	UPROPERTY(EditAnywhere, Category = "Luck Levels")
		int arrows_given_2 = 8;
	UPROPERTY(EditAnywhere, Category = "Luck Levels")
		int arrows_given_3 = 12;
};

UCLASS()
class DUNGEONS_API UPoisonArrowGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UPoisonArrowGameplayEffect(const FObjectInitializer& ObjectInitializer);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
		float damagePerSecond = 15.0f;
};