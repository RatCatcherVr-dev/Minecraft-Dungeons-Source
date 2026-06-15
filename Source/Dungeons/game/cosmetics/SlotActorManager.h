#pragma once

#include "CoreMinimal.h"
#include "SlotActorManager.generated.h"

UCLASS(DefaultToInstanced)
class USlotActorManager : public UObject {
	GENERATED_BODY()

public:
	void Register(AActor* owner, UObject* slot);
	
	void DestroyActor(UObject* slot);

	void RemoveSlot(UObject* slot);

	UChildActorComponent* AssignActorClass(UObject* slot, TSubclassOf<AActor> actorClass);

	TMap<UObject*, UChildActorComponent*> GetActors() const;

	void EmptyActors();
private:
	UPROPERTY()
	TMap<UObject*, UChildActorComponent*> SlotActors;	
};