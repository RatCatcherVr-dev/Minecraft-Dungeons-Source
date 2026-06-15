#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayPrediction.h"
#include "CooldownProvider.generated.h"

struct FGameplayEffectSpec;
struct FPredictionKey;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta=(CannotImplementInterfaceInBlueprint))
class UCooldownProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DUNGEONS_API ICooldownProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable)
	virtual bool IsOnCooldown() const { return false;  }
	UFUNCTION(BlueprintCallable)
	virtual float GetCooldownSecondsRemaining() const { return 0; }
	UFUNCTION(BlueprintCallable)
	virtual float GetCooldownFractionRemaining() const { return 0; }
	UFUNCTION(BlueprintCallable)
	virtual float GetCooldownSecondsDuration() const { return 0; }

	virtual TPair<float,float> GetCooldownSecondsRemainingAndDuration() const { return TPair<float,float>(0.f, 0.f); };
	
	virtual void TriggerCooldown(float seconds, FPredictionKey key = FPredictionKey()) {};
	virtual void TriggerCooldown(float seconds, float reduction, FPredictionKey key = FPredictionKey()) {};
	virtual void ModifyCooldown(float offsetSeconds) {};
	virtual void ResetCooldown() {};
};
