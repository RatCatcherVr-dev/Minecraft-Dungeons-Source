#pragma once

#include "GameFramework/Actor.h"
#include "game/item/ItemTypeDefs.h"
#include "game/component/PaperdollComponent.h"
#include "GearActor.generated.h"

UCLASS(Blueprintable)
class DUNGEONS_API AGearActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGearActor();

	void SetVisibility(bool visible);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPaperdollComponent* Paperdoll;

protected:
	void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	ESlotType SlotType;
private:
	UFUNCTION()
	void OnDetachedComponentCreated(USceneComponent* component);

	UPROPERTY()
	TArray<USceneComponent*> DetachedComponents;
};

UCLASS()
class DUNGEONS_API AItemActor : public AGearActor {
	GENERATED_BODY()
public:
	AItemActor();

	static AItemActor* SpawnItemActor(UWorld* world, TSubclassOf<class AItemActor> itemClass);
};
