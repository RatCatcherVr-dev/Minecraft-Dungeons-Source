#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReplicatedInteractInParentComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DUNGEONS_API UReplicatedInteractInParentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UReplicatedInteractInParentComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UFUNCTION()
	void InteractInParentActor(ACharacter* Instigator);
};
