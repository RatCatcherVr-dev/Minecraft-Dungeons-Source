// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "game/item/SerializableItemId.h"
#include "util/FloatRange.h"
#include "game/item/ItemSlot.h"
#include <GameplayModMagnitudeCalculation.h>
#include "SoulComponent.generated.h"

class AGearItemInstance;
class AMobCharacter;

DECLARE_MULTICAST_DELEGATE(FOnSoulAbsorbed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulCountDelta, int, deltaSoulPower);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSoulCountNeededHint, int, neededSouls, float, percentageNeeded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulPercentageChanged, USoulComponent*, soulComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulCollectingStarted, USoulComponent*, soulComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulCollectingStopped, USoulComponent*, soulComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnyMobKilled, AActor*, killedBy);

UCLASS()
class DUNGEONS_API UAddPlainSoulModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UAddPlainSoulModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};

UCLASS()
class DUNGEONS_API UAddSoulModCalculation : public UGameplayModMagnitudeCalculation {
	GENERATED_BODY()
public:
	UAddSoulModCalculation();
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	const FGameplayEffectAttributeCaptureDefinition SoulGatheringCapture;
	const FGameplayEffectAttributeCaptureDefinition SoulGatheringMultiplierCapture;
};

UCLASS()
class DUNGEONS_API UBaseGenerateSoulsGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBaseGenerateSoulsGameplayEffect();
};

UCLASS()
class DUNGEONS_API UGenerateSoulsGameplayEffect : public UBaseGenerateSoulsGameplayEffect {
	GENERATED_BODY()
public:
	UGenerateSoulsGameplayEffect();
};


UCLASS()
class DUNGEONS_API UModifySoulsGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UModifySoulsGameplayEffect();

	static const FName SoulKey;
};

UCLASS( ClassGroup=(Custom), Within=BaseCharacter, meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API USoulComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type endPlayReason) override;
public:
	using TargetProvider = TFunction<FVector()>;

	USoulComponent();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddSouls(int amount);


	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetSoulCount() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	int GetMaxSoulCount() const;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	float GetSoulPercentage() const;

	void TriggerSoulsNeededHint(int soulCount) const;

	UPROPERTY(BlueprintAssignable)
	FOnSoulCountDelta OnSoulCountDelta;

	UPROPERTY(BlueprintAssignable)
	FOnSoulCountNeededHint OnSoulCountNeededHint;

	UPROPERTY(BlueprintAssignable)
	FOnSoulPercentageChanged OnSoulPercentageChanged;

	UPROPERTY(BlueprintAssignable)
	FOnSoulCollectingStarted OnSoulCollectingStarted;

	UPROPERTY(BlueprintAssignable)
	FOnSoulCollectingStopped OnSoulCollectingStopped;

	UPROPERTY(BlueprintAssignable)
	FOnAnyMobKilled OnAnyMobKilled;

	UFUNCTION(BlueprintCallable)
	bool IsCollecting() const;

	/** Spawns a soul on Client from Server. Use when on server and need to spawn a soul. */
	void ServerSpawnSoul(const FVector& spawnPosition, const int amount = 1, float riseTimeMin = 1.0f, float riseTimeMax = 1.0f);
	void ServerSpawnSoulWithBaseGE(TSubclassOf<UBaseGenerateSoulsGameplayEffect> generateSoulsGE, const FVector& spawnPosition, const int amount = 1, float riseTimeMin = 1.0f, float riseTimeMax = 1.0f);

	static float CalculateSoulTravelTime(const FVector& startPosition, const FVector& endPosition);

	FOnSoulAbsorbed OnAbsorbedSoul;

	int GetCurrentSoulGatherAmount() const;

protected:
	/** Spawns a soul locally. I.e on the machine that called the function. */
	void SpawnSoul(const FVector& spawnPosition, float travelTime, const FRandomStream& rand, int amount = 1, FloatRange riseTime = FloatRange(1.0f, 1.0f));

	UFUNCTION(Client, Reliable)
	void ClientSpawnSoul(const FVector& spawnPosition, float travelTime, int amount = 1, float riseTimeMin = 1.0f, float riseTimeMax = 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<class ASoul> SoulClass;

	void OnSoulCountChanged(const FOnAttributeChangeData&);

	void OnMaxSoulCountChanged(const FOnAttributeChangeData&);

	void OnSoulGatheringChanged(const FOnAttributeChangeData&);

private:
	ABaseCharacter* GetCharacterOwner() const;
	class UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	void UpdateIsCollecting();

	void OnAttemptChangeSouls(const FGameplayEffectModCallbackData&);

	void ServerSpawnSoulInternal(const FVector& spawnPosition, FGameplayEffectSpec captureSpec, const int amount = 1, float riseTimeMin = 1.0f, float riseTimeMax = 1.0f);

	void OnAbsorbSouls(FGameplayEffectSpec, int);

	void AddSouls(int amount);

	void SpawnSoulWrapper(TargetProvider target, FGameplayEffectSpec captureSpec, FRandomStream rand, const int amount = 1, float riseTimeMin = 1.0f, float riseTimeMax = 1.0f);

	UFUNCTION()
	void OnAnyPlayerKilledMob(const AMobCharacter* actorKilled, AActor* byActor, AActor* byWhat);

	TArray<TSoftObjectPtr<UItemSlot>> SoulSlots;

	int mLastSeenSoulCount = 0;

	bool mWasCollecting = false;
};
