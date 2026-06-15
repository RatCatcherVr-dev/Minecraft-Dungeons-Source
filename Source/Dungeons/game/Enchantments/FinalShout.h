// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include <GameplayEffect.h>
#include "FinalShout.generated.h"

UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UFinalShoutGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFinalShoutGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UFinalShout : public UEnchantment
{
	GENERATED_BODY()

	UFinalShout();

	FText CreateDescription() const override;


protected:
	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	void OnMakeActivatable();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UFinalShoutGameplayEffect> Effect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float triggerCooldownBase = 12.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float triggerCooldownPerLevel = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float triggerHealthFractionThreshold = 0.25f;

private:	
	bool ReadyToActivate() const;

	FTimerHandle Handle;
};
