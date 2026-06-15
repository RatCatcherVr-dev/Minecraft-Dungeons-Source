// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "GameplayEffect.h"
#include "game/actor/item/TogglableItemInstance.h"
#include "game/component/GrowAttackComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/item/enchantment/EnchantmentData.h"
#include "EnchantersTome.generated.h"

UCLASS()
class DUNGEONS_API AEnchantersTome : public ATogglableItemInstance
{
	GENERATED_BODY()
	
public:
	AEnchantersTome();
	
	void Toggle(const FPredictionKey& predictionKey, bool Enabled) override;
	void Tick(float DeltaTime) override;
	bool GetApplicableCharacters(TArray<ABaseCharacter*>& outCharacters);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UGrowAttackBuffGameplayEffect> Effect;

	void OnToggleStart(const FPredictionKey& predictionKey);
	void OnToggleEnd(const FPredictionKey& predictionKey);

	void DisenchantTarget(TWeakObjectPtr<ABaseCharacter> target);
	void EnchantTarget(ABaseCharacter* target);

	float GetDuration() const;

	void AddAbsoluteEnchantments(TArray<FEnchantmentData>& outEnchantments);
	FEnchantmentData GetRandomPetCapableEnchantment() const;
	FEnchantmentData GetRandomPlayerCapableEnchantment() const;

	UFUNCTION(NetMulticast, Reliable)
	void SpawnTomeMulticast();
	UFUNCTION(NetMulticast, Reliable)
	void DestroyTomeMulticast();

private:

	FTimerHandle InterruptTimer;

	TMap<ABaseCharacter*, TArray<FEnchantmentData>> CachedCharacterMysteryEnchants;
	TArray<TWeakObjectPtr<ABaseCharacter>> CurrentTargets;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int MaxTargetCount = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float TargetRange = 1400.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float TargetBreakRange = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int TotalAbsoluteEnchantsToApply = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	int EnchantmentLevel = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AActor> TomeActorClass = nullptr;

	TWeakObjectPtr<AActor> TomeActor = nullptr;
};
