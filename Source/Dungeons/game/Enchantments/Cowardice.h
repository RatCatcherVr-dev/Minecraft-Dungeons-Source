// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "GameplayEffect.h"
#include "Cowardice.generated.h"

class UHealthComponent;

UCLASS()
class DUNGEONS_API UCowardiceGameplayEffect : public UGameplayEffect {
	GENERATED_BODY()
public:
	UCowardiceGameplayEffect(const FObjectInitializer& ObjectInitializer);
};


UCLASS()
class DUNGEONS_API UCowardice : public UEnchantment
{
	GENERATED_BODY()
public:
	UCowardice();

protected:
	void OnStart() override;
	void OnEnd() override;
	void OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UCowardiceGameplayEffect> Effect;
private:
	bool HasFullHealth() const;
	bool IsEffectActive() const;
	void ApplyEffect();
	void RemoveEffect();

	void OnHeal(float amount);

	UHealthComponent* HealthComponent = nullptr;

	FActiveGameplayEffectHandle EffectHandle;
};
