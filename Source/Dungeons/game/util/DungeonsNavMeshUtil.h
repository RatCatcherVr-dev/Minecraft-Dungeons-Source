#pragma once

#include <Kismet/BlueprintFunctionLibrary.h>
#include "DungeonsNavMeshUtil.generated.h"

UCLASS(BlueprintType)
class UDungeonsNavMeshUtil : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void UpdateSceneComponentNavMesh(USceneComponent* componentToUpdate) {
		FNavigationSystem::OnComponentTransformChanged(*componentToUpdate);
	}
};
