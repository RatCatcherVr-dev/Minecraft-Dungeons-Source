#pragma once

#include "game/item/instance/AItemInstance.h"
#include "game/actor/item/GearActor.h"
#include <Animation/AnimInstance.h>
#include "GearItemInstance.generated.h"

UCLASS()
class DUNGEONS_API AGearItemInstance : public AItemInstance
{
	GENERATED_BODY()
public:
	
	virtual void BeginPlay() override;

	/** The Actor to spawn client side for this gear item. */
	UPROPERTY(EditDefaultsOnly, Category = "Dungeons")
	TSubclassOf<AGearActor> GearActor = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Dungeons|Animation")
	TSubclassOf<UAnimInstance> OverrideAnimationsInstance = nullptr;

private:
	void CheckEquippedEffects();
};
