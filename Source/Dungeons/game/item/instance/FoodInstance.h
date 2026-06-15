// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include <GameplayEffect.h>
#include "FoodInstance.generated.h"


UCLASS()
class DUNGEONS_API UFoodHealingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFoodHealingGameplayEffect();

	const FGameplayTag& GetTriggerTag() const;
};

UCLASS(BlueprintType)
class DUNGEONS_API UBreadHealingGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	UBreadHealingGameplayEffect();
};

UCLASS(BlueprintType)
class DUNGEONS_API UAppleHealingGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	UAppleHealingGameplayEffect();
};

UCLASS(BlueprintType)
class DUNGEONS_API UPorkHealingGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	UPorkHealingGameplayEffect();
};

UCLASS(BlueprintType)
class DUNGEONS_API USalmonHealingGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	USalmonHealingGameplayEffect();
};

UCLASS(BlueprintType)
class DUNGEONS_API USweetBerryHealingGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	USweetBerryHealingGameplayEffect();
};

UCLASS(BlueprintType)
class DUNGEONS_API USweetBerrySwiftnessGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	USweetBerrySwiftnessGameplayEffect();
};

UCLASS(BlueprintType)
class DUNGEONS_API UTropicalFishHealingGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	UTropicalFishHealingGameplayEffect();
};

UCLASS(BlueprintType)
class DUNGEONS_API UTropicalFishOxygenGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	UTropicalFishOxygenGameplayEffect();
};

UCLASS(BlueprintType)
class DUNGEONS_API UChorusFruitHealingGameplayEffect : public UFoodHealingGameplayEffect {
	GENERATED_BODY()
public:
	UChorusFruitHealingGameplayEffect();
};

UCLASS()
class DUNGEONS_API AFoodInstance : public AItemInstance
{
	GENERATED_BODY()
	
public:
	void Activate(const FPredictionKey& predictionKey) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UFoodHealingGameplayEffect> FoodEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TArray<TSubclassOf<UFoodHealingGameplayEffect>> AdditionalEffects;
};
