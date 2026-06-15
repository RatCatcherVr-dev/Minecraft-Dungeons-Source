// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TelemetryUploadWidget.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API UTelemetryUploadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Telemetry")
	void LogGameSessionUserGrade(int grade);


	
	
};
