#pragma once

#include <GameFramework/Actor.h>
#include <Components/ActorComponent.h>
#include "InstancedMeshesActor.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class UInstancedStaticMeshComponent;

USTRUCT()
struct FInstancedMeshesActorNameList
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FName> NameList;
};



UCLASS(NotBlueprintable, NotPlaceable)
class DUNGEONS_API AInstancedMeshesActor : public AActor {
	GENERATED_BODY()
public:

	AInstancedMeshesActor(const FObjectInitializer& ObjectInitializer);

	void SetHierarchical(bool);

	void Place(UStaticMesh*, const FTransform&, bool);
	void Place(UStaticMeshComponent*, const FTransform&, bool);

	void SpawnStaticMeshesFromContainedInstances();

	void Serialize(FArchive& Ar);

private:
	UInstancedStaticMeshComponent& _GetOrCreateInstancedMeshFor(UStaticMesh*, UStaticMeshComponent*, bool);

	UPROPERTY(VisibleInstanceOnly)
	TArray< UInstancedStaticMeshComponent* > mInstances; //primarily to prevent GC

	TMap<UStaticMesh*, TMap<FString, UInstancedStaticMeshComponent* > > mInstanceLookup;


#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly)
	TMap< UInstancedStaticMeshComponent* , FInstancedMeshesActorNameList > mInstancesNames;
#endif
	

	bool mHierarchical = true;
};
