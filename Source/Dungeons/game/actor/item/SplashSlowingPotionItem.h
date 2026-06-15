// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "game/actor/item/ThrownItem.h"
#include "GameplayEffect.h"
#include "SplashSlowingPotionItem.generated.h"

UCLASS()
class DUNGEONS_API USPlashSlowingPotionGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	USPlashSlowingPotionGameplayEffect();
};

/**
*
*/
UCLASS()
class DUNGEONS_API ASplashSlowingPotion : public AThrownItem
{
	GENERATED_BODY()

public:
	ASplashSlowingPotion(const class FObjectInitializer& OI);

	void BeginPlay() override;

	void OnHitObject(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dungeons")
	TSubclassOf<USPlashSlowingPotionGameplayEffect> Effect;
};
