

#pragma once

#include "CoreMinimal.h"
#include "game/item/instance/AItemInstance.h"
#include "game/component/movement/CustomMover.h"
#include "game/component/PlayerCharacterMovementComponent.h"
#include "CustomMoverItemInstance.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class DUNGEONS_API ACustomMoverItemInstance : public AItemInstance
{
	GENERATED_BODY()

	void OnCustomMoverActivated(ECustomMoverSlot slot);
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
public:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void Activate(const FPredictionKey& predictionKey) override;

	UFUNCTION()
	virtual void OnRep_Slot();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCustomMover> CustomMover;

	UPROPERTY(ReplicatedUsing = OnRep_Slot, Transient)
	ECustomMoverSlot Slot;

protected:

	virtual void OnMoverTriggered();
};
