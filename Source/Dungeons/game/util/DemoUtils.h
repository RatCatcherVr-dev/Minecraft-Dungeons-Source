// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "DemoUtils.generated.h"

/**
 *
 */
UCLASS()
class DUNGEONS_API UDemoUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Demo")
	static bool IsDemo();

	UFUNCTION(BlueprintCallable, Category = "Dungeons|Demo")
	static bool TryStartDemo(const UObject* WorldContextObject);

	// D11.SSN
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Dungeons|Splash")
	static bool SkipSplash();
};


