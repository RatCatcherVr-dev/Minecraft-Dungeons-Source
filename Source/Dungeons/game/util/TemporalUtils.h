// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "TemporalUtils.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UTemporalUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Time Dilation")
	static ATemporalDilationNormalizer* DilateTime(const UObject* WorldContextObject, const float dilation, TArray<AActor*> targets, float seconds = 1.f, bool dilateSoound = true);
};

UCLASS()
class DUNGEONS_API ATemporalDilationNormalizer : public AActor {
	GENERATED_BODY()
public:
	void Start(const UObject* worldContextObject, TArray<AActor*> targets, float dilation, float seconds);
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Time Dilation")
	void Affect(AActor* actor);
	UFUNCTION(BlueprintCallable, Category = "Dungeons|Time Dilation")
	void Unaffect(AActor* actor);
protected:
	virtual void normalize();
	UObject* WorldContextObject;
	float Seconds;
	FTimerHandle TimerHandle;
	float StartTimerstamp;
	float Dilation;
	TArray<AActor*> Targets;
};