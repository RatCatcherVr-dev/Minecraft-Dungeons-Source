#pragma once

#include "CoreMinimal.h"

#include "game/level/InstancedMeshBase.h"
#include "StaticMeshCollection.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AStaticMeshCollection : public AInfo {
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TMap<FName, TSubclassOf<AInstancedMeshBase>> StaticMeshMap;

};
