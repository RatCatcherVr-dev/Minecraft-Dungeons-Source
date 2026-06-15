// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/IntervalExecutionEnchantment.h"
#include "GravityPulse.generated.h"

class ABaseCharacter;

UCLASS()
class DUNGEONS_API UGravityPulse : public UIntervalExecutionEnchantment {
	GENERATED_BODY()
public:
	UGravityPulse();

	FText CreateDescription() const override;

	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
protected:
	void Execution() override;

	float GetExecutionInterval() const override;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float BaseRadius = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Dungeons")
	float PulseInterval = 5.0f;
private:
	void OnPulse();

	void TryStartPullOnCharacter(TWeakObjectPtr<ABaseCharacter> character, UAbilitySystemComponent* ownerAbilitySystem);

	void EndPullOnCharacter(TWeakObjectPtr<ABaseCharacter> character);

	TArray<TWeakObjectPtr<ABaseCharacter>> PullingCharacters;
};
