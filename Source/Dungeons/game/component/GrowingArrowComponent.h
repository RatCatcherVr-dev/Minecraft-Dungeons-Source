// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrowingArrowComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UGrowingArrowComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UGrowingArrowComponent();
	
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Activate(bool reset) override;

	void SetMaxDistance(float distance) { MaxDistance = distance; }

	void SetMultiplier(float multiplier);

	float GetCurrentAlpha() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float MaxScale = 3.f;
private:	
	float MaxDistance = 1600.0f;

	FVector LastLocation;
	float DistanceTraveled = 0.f;
	float Multiplier = 1.f;
};
