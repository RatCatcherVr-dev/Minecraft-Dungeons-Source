// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "InterruptableItemInstance.generated.h"

/**
 * 
 */
UCLASS()
class DUNGEONS_API AInterruptableItemInstance : public AItemInstance
{
	GENERATED_BODY()

	//Will be called both on server and owning client
	virtual void Interrupt() {};
	virtual bool IsActive() const { return false; }

	void OnStunned(const FGameplayTag tag, const int32 tagCount);
	void Interrupt_Internal();

public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// do not use on toggleable items
	void SetAllowInterrupt(bool allow) { allowInterrupt = allow; };
	bool GetAllowInterrupt() { return allowInterrupt; };

protected:
	// do not use on toggleable items
	bool allowInterrupt = true;
};
