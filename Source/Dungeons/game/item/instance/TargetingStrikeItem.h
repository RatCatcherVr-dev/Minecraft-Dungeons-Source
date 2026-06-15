// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "game/actor/TargetingActor.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/actor/item/InterruptableItemInstance.h"
#include "game/input/TargetController.h"
#include "game/actor/item/TogglableItemInstance.h"
#include "TargetingStrikeItem.generated.h"



UCLASS()
class DUNGEONS_API UTargetingStrikeDamageItemGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UTargetingStrikeDamageItemGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UTargetingStrikeItemGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UTargetingStrikeItemGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class ALightingRodReticule : public AActor {

	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadOnly)
	FColor Color = FColor::White;

	UPROPERTY(BlueprintReadOnly)
	bool IsLocalCoop = false;
public:	
	ALightingRodReticule();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCharged();

	void SetColor(FColor color) {
		Color = color;
		IsLocalCoop = Color != FColor::White;
	}
};

UCLASS()
class DUNGEONS_API ATargetingStrikeItem : public ATogglableItemInstance
{
	GENERATED_BODY()
	
public:
	ATargetingStrikeItem();

	void Toggle(const FPredictionKey& predictionKey, bool Enabled) override;

	float GetStats(EItemStats stat) const;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float StrikeDamagePerSoul = 60.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float StrikeIntervalSeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float StrikeRadius = 350.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float PreStrikeDelaySeconds = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DefaultTargetOffset = 600.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<ALightingRodReticule> TargetingActorClass;

	UPROPERTY()
	ALightingRodReticule* CurrentTargetingActor = nullptr;

private:	
	void SpawnTargetingActor();

	UFUNCTION(Server, Unreliable, WithValidation)
	void ServerStartStrike(FVector strikeLocation);

	void Strike(FVector strikeLocation);

	void OnCharged();

	FTimerHandle PreStrikeTimerHandle;
	FTimerHandle StrikeTimerHandle;
};