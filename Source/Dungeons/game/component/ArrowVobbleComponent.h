// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArrowVobbleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UArrowVobbleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UArrowVobbleComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void StartVobble();
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float VobbleTimeSeconds = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	float VobbleScale = 10.0f;
private:	
	bool bCanVobble = false;

	float StartTimeStamp;
	FRotator InitialRotation;

	TWeakObjectPtr<USceneComponent> TargetComponent;
};
