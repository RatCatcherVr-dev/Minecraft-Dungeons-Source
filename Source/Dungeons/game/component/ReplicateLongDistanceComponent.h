// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ReplicateLongDistanceComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DUNGEONS_API UReplicateLongDistanceComponent : public UActorComponent 
{
	GENERATED_BODY()

public:
	UReplicateLongDistanceComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CullDistanceSquared = 225000000.0;

private:
};
