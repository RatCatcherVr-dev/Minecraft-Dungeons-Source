#pragma once

#include <GameFramework/Actor.h>
#include <Components/ActorComponent.h>
#include "InstancedMeshBase.generated.h"

// D11.DB - Base BP class used by the StaticMeshSpawner.
UCLASS(BlueprintType)
class DUNGEONS_API AInstancedMeshBase : public AActor {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void Place(const FTransform& transform, const FString& filter = TEXT(""));
};
