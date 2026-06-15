// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include <GameplayModMagnitudeCalculation.h>
#include "Kismet/BlueprintFunctionLibrary.h"
#include "game/actor/EffectGroup/EffectGroup.h"
#include "Blind.generated.h"

UCLASS()
class DUNGEONS_API UBlindCloudSpawnHelper : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static ABlindCloud* SpawnBlindCloud(TSubclassOf<class AActor> blindCloudClass, TSubclassOf<class UGameplayEffect> gameplayEffect, AActor* owner, const FVector& location, float duration, bool squid = false);
private:
	static FGameplayEffectSpec CreateBlindCloudEffectSpec(TSubclassOf<UGameplayEffect> gameplayEffect, AActor* owner, const FVector& location, ABlindCloud* blindCloud, float duration);

	static float MeleeHitChance;
	static float RangeHitChance;
};

UCLASS()
class DUNGEONS_API UBlindGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UBlindGameplayEffect(const FObjectInitializer& ObjectInitializer);
};
UCLASS()
class DUNGEONS_API ABlindCloud : public AActor
{
	GENERATED_BODY()
public:
	ABlindCloud();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USphereComponent* Sphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool Squid;

	UPROPERTY(BlueprintReadWrite)
	float CloudDuration;

	void BeginPlay() override;

	void NotifyActorBeginOverlap(AActor* OtherActor) override;

	void SetGameplaySpec(FGameplayEffectSpec&& spec);

	class ABaseCharacter* GetCharacterOwner() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Blind Cloud")
		void OnPlayerCharacterEnteredNonFriendlyCloud(APlayerCharacter* Player);
	UFUNCTION(BlueprintImplementableEvent, Category = "Blind Cloud")
		void OnPlayerCharacterLeftNonFriendlyCloud(APlayerCharacter* Player);


	void NotifyActorEndOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Blind Cloud")
	TArray<APlayerCharacter*> PlayersInside;
private:
	FGameplayEffectSpec Spec;

	
};