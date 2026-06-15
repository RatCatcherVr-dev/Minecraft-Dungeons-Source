#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "game/actor/item/StorableItem.h"
#include "game/actor/item/ItemEmitterActor.h"
#include "game/item/ItemType.h"
#include "Components/ArrowComponent.h"
#include "ElytraSpawnTriggerBox.generated.h"

UCLASS()
class DUNGEONS_API AElytraSpawnTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	AElytraSpawnTriggerBox();
	void NotifyActorBeginOverlap(AActor* OtherActor) override;

private:
	TWeakObjectPtr<AStorableItem> spawnedItem;

	UPROPERTY(EditInstanceOnly)
	UArrowComponent* Spawnpoint;
};