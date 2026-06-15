// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/Enchantments/Enchantment.h"
#include "Echo.generated.h"


UCLASS()
class DUNGEONS_API UEcho : public UEnchantment
{
	GENERATED_BODY()
public:
	UEcho();

	void OnAfterMeleeAttack(AActor* attackTarget, int32 hitCount, FVector attackVector, int32 index, FRandomStream& randStream, FSharedPredictionContext window) override;

	// Time until echo can trigger after attack again, scales by level: (Time - (Level - 1))
	const float BaseTriggerEchoTimeSeconds = 5.0f;
	const float MaxDelay = 0.2f;

	const float MobTriggerEchoTimeSeconds = 2.0f;
private:
	void OnCanTriggerEcho();

	void OnEcho(TWeakObjectPtr<AActor> target, FVector attackVector, int32 index, int32 seed, FSharedPredictionContext context);

	FTimerHandle ResetEchoTimerHandle;

	bool bCanTrigger = true;
	bool bIsInEcho = false;
};
