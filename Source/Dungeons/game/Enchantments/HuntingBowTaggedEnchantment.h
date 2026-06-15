// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include <GameplayEffectTypes.h>
#include "HuntingBowTaggedEnchantment.generated.h"

class ABaseCharacter;

UCLASS() // huntingbow
class DUNGEONS_API UTaggedGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UTaggedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UHuntingBowTaggedEnchantment : public UEnchantment
{
	GENERATED_BODY()

public:
	UHuntingBowTaggedEnchantment();

	void OnBeforeReceivedDamage(bool& outAttackMissed, struct FGameplayEffectModCallbackData &data, UAbilitySystemComponent* targetComponent, FRandomStream& randStream, FPredictionKey key) override;

	void SetTaggedByCharacter(ABaseCharacter* taggedBy);

	void OnStart() override;
	void OnEnd() override;
private:
	TWeakObjectPtr<ABaseCharacter> TaggedByCharacter;
	FActiveGameplayEffectHandle Handle;
};
