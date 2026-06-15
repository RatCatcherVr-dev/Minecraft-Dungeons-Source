// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueManager.h"
#include "DungeonsGameplayCueManager.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UDungeonsGameplayCueManager : public UGameplayCueManager
{
	GENERATED_BODY()
	
	void OnEngineInitComplete() override;
	void InvokeGameplayCueExecuted_FromSpec(UAbilitySystemComponent* OwningComponent, const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey) override;

	UFUNCTION(BlueprintCallable, Category = "Dungeons")
	static FVector GetFullNormalFromCueParameters(const FGameplayCueParameters& SourceParams);

	bool ProcessPendingCueExecute(FGameplayCuePendingExecute& PendingCue) override;

public:
	void RefreshExecutionAccelerationSet();
	
private:
	void OnRutimeCueSetLoaded(TArray<FSoftObjectPath>);

	TSet<FGameplayTag> OnExecuteAccelerationSet;
	
};
