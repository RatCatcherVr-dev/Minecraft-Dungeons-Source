// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpringArmComponent.h"
#include "LovikaSpringArmComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API ULovikaSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	/** Update this value to interpolate arm length to a different length */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera)
	float SeekArmLength;

	/** Lag speed for arm length */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ArmLagSpeed;

	
	

	
	
	/** Returns whether a particular section is currently casting shadows */
	UFUNCTION(BlueprintCallable, Category = Camera)
	void Reinitialize();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION(BlueprintCallable)
	void SetDesiredArmLegnth(float Length);

	UFUNCTION(BlueprintCallable)
	void ResetDesiredArmLength();

	void SetXYSnapAmount(float snapAmount, float snapRestoreDuration);

protected:
	float InternalTargetLength;

	/** Updates the desired arm location, calling BlendLocations to do the actual blending if a trace is done */
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;

private:

	void UpdateSnapAmount(float DeltaTime);

	float XYSnapAmount = 0.0f;
	float XYSnapRestoreDuration = 1.0f;

};
