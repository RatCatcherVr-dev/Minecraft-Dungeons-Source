// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include <GameplayModMagnitudeCalculation.h>
#include "game/component/HealthComponent.h"
#include "game/component/WalletComponent.h"
#include "game/abilities/effects/InvulnerableGameplayEffect.h"
#include "DeathBarter.generated.h"

UCLASS()
class DUNGEONS_API UDeathBarterChargingGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDeathBarterChargingGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDeathBarterChargingLevelOneGameplayEffect : public UDeathBarterChargingGameplayEffect {
	GENERATED_BODY()
public:
	UDeathBarterChargingLevelOneGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDeathBarterChargingLevelTwoGameplayEffect : public UDeathBarterChargingGameplayEffect {
	GENERATED_BODY()
public:
	UDeathBarterChargingLevelTwoGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDeathBarterChargingLevelThreeGameplayEffect : public UDeathBarterChargingGameplayEffect {
	GENERATED_BODY()
public:
	UDeathBarterChargingLevelThreeGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDeathBarterActiveGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDeathBarterActiveGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDeathBarterReviveGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UDeathBarterReviveGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UDeathBarterInvulnerabilityGameplayEffect : public UInvulnerableGameplayEffect {
	GENERATED_BODY()
public:
	UDeathBarterInvulnerabilityGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UDeathBarter : public UEnchantment
{
	GENERATED_BODY()
public:
	UDeathBarter();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type reason) override;

	void OnEmeraldsEarned(int32 amount, bool TriggerCue = true);

	void OnBeforeDeath();

	UFUNCTION(Client, Unreliable)
	void Client_OnBeforeDeath();

	TSubclassOf<UDeathBarterChargingGameplayEffect> GetChargingEffect() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USoundCue* OnEmeraldCollectedSound;

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UDeathBarterChargingGameplayEffect>> ChargingEffects;

	UPROPERTY(EditDefaultsOnly)
	float InvulnerabilityDuration = 3.f;

private:
	UFUNCTION(Server, WithValidation, Reliable)
	void ServerSendCount(int32 count);

	int32 AttemptConsumeCurrency(const FItemId&, int32, ECurrencyObtainReason);

	void OnActiveChanged(const FGameplayTag, int32);

	void ReplicateCountToServer();

	//Used internally to consume emeralds
	int32 LocalCounter = 0;

	//Used when we are not authorative
	int32 BatchCount = 0;

	FTimerHandle ReplicationTimer;

	int32 PlaySoundCount = 0;
};