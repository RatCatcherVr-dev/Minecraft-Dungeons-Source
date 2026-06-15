// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Frenzied.generated.h"

UCLASS()
class DUNGEONS_API UFrenziedGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UFrenziedGameplayEffect(const FObjectInitializer& ObjectInitializer);
};

UCLASS()
class DUNGEONS_API UFrenzied : public UEnchantment
{
	GENERATED_BODY()
public:
	UFrenzied();

	FText CreateDescription() const override;

	void OnStart() override;

	void OnEnd() override;
	
	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<UFrenziedGameplayEffect> FrenziedEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float TriggerThreshold = 0.5f;

private:
	bool GetShouldTrigger() const;
	bool IsEffectActive() const;
	void ApplyEffect();
	void RemoveEffect();

	void OnHeal(float amount);

	FActiveGameplayEffectHandle EffectHandle;
};
