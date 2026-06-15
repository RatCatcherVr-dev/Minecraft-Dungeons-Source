// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "game/actor/TargetingActor.h"
#include "game/component/HealthComponent.h"
#include "game/abilities/effects/BaseItemDamageGameplayEffect.h"
#include "game/actor/item/TogglableItemInstance.h"
#include "game/util/Pushback.h"
#include "CorruptedBeacon.generated.h"


UCLASS()
class DUNGEONS_API UCorruptedBeaconGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCorruptedBeaconGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UCorruptedBeaconDamageGameplayEffect : public UBaseItemDamageGameplayEffect {
	GENERATED_BODY()
public:
	UCorruptedBeaconDamageGameplayEffect();
};

UCLASS()
class DUNGEONS_API ACorruptedBeacon : public ATogglableItemInstance
{
	GENERATED_BODY()
	
public:
	ACorruptedBeacon();

	void Tick(float DeltaTime) override;

	float GetStats(EItemStats stat) const;

	void Toggle(const FPredictionKey& predictionKey, bool Enabled) override;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float DamagePerSoulPerTick = 25.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UCorruptedBeaconGameplayEffect> Effect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FPushback OnKillPushback;

	UPROPERTY(EditDefaultsOnly)
	class UAnimMontage* Montage = nullptr;
};
