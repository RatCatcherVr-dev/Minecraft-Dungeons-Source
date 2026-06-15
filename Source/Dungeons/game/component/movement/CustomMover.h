

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "game/util/Pushback.h"
#include "CustomMover.generated.h"

class UPlayerCharacterMovementComponent;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class DUNGEONS_API UCustomMover : public UObject
{
	GENERATED_BODY()

public:
	//When needed we should extend this to take some meaningful arguments.
	UFUNCTION(BlueprintNativeEvent)
	void ExecuteCustomMove(UPlayerCharacterMovementComponent* Component) const;

	virtual bool ShouldTriggerLeniencyWindow() const { return false; };
};


UCLASS(BlueprintType, Blueprintable)
class DUNGEONS_API UPushbackCustomMover : public UCustomMover
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	FPushback Pushback;

	UPROPERTY(EditDefaultsOnly)
	bool bApplyResistance = true;

	UPROPERTY(EditDefaultsOnly)
	float UnderWaterMultiplier = 0.5f;
	
	void ExecuteCustomMove_Implementation(UPlayerCharacterMovementComponent* Component) const override;

	bool ShouldTriggerLeniencyWindow() const override { return true; }
};
