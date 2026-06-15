// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObjectiveInterestPoint.generated.h"

UCLASS()
class DUNGEONS_API AObjectiveInterestPoint : public AActor {
	GENERATED_BODY()
public:	
	AObjectiveInterestPoint();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Dungeons")
	class UMapPinComponent* MapPinComponent;
};
