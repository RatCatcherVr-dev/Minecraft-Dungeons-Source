// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "game/item/instance/AItemInstance.h"
#include "game/actor/TargetingActor.h"
#include "ThrowableItemInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API AThrowableItemInstance : public AItemInstance
{
	GENERATED_BODY()
	
public:
	AThrowableItemInstance();

	bool TryActivate() override;

	void TargetingEnded(FVector target, bool outcome);

	void ResetInstance() override;

	UFUNCTION()
	virtual FVector CalculateVelocityAmountToLoc(FVector &location, float maxHeight);
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<ATargetingActor> TargetingActorClass;
	UPROPERTY()
	TWeakObjectPtr<ATargetingActor> CurrentTargetingActor = nullptr;
	bool IsAiming = false;
	FVector Target;
private:
	UFUNCTION(Server, Reliable, WithValidation, Category = "Dungeons")
	void SetTargetServer(FVector target);

};
