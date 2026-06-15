#pragma once

#include "game/actor/item/ThrownItem.h"
#include "EnderPearlItem.generated.h"

UCLASS()
class DUNGEONS_API AEnderPearlItem : public AThrownItem
{
	GENERATED_BODY()

	void BeginPlay() override;

	void OnHitObject(class UPrimitiveComponent* ThisComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
};
