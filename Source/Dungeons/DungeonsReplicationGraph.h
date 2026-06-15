#pragma once
#include "CoreMinimal.h"
#include "ReplicationGraph.h"
#include "DungeonsReplicationGraph.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDungeonsReplicationGraph, Log, All);

enum class EClassRepPolicy : uint8
{
	NotConfigured, //Was never set anywhere.
	NotRouted, //Not replicated.
	RelevantAllConnections, //Always replicated
	Spatialize_Static,//Frequent update or, every frame
	Spatialize_Dynamic,//Every frame
	Spatialize_Dormancy,//Once, usually. Can switch to either option. Decided by AActor::NetDormancy.
};

class UReplicationGraphNode_ActorList;
class UReplicationGraphNode_GridSpatialization2D;

UCLASS(Transient, config=Engine)
class DUNGEONS_API UDungeonsReplicationGraph : public UReplicationGraph
{
public:
	GENERATED_BODY()

	virtual void ResetGameWorldState() override;
	virtual void InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager) override;
	virtual void InitGlobalActorClassSettings() override;
	virtual void InitGlobalGraphNodes() override;
	virtual void RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo) override;
	virtual void RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo) override;
	virtual void AddNetworkActor(AActor* Actor) override;

	UPROPERTY()
	TArray<UClass*> SpatializedClasses;

	UPROPERTY()
	TArray<UClass*> AlwaysRelevantClasses;

	UPROPERTY()
	UReplicationGraphNode_GridSpatialization2D* GridNode;

	UPROPERTY()
	UReplicationGraphNode_ActorList* AlwaysReleventNode;

	/** Maps the actors the needs to be always relevant across streaming levels */
	TMap<FName, FActorRepListRefView> AlwaysRelevantStreamingLevelActors;

protected:
		
	FORCEINLINE bool IsSpatialized(EClassRepPolicy Mapping)
	{
		// true for rep policies where location should matter
		return Mapping >= EClassRepPolicy::Spatialize_Static && Mapping <= EClassRepPolicy::Spatialize_Dormancy;
	}

	//Gets mapping for a class.
	EClassRepPolicy GetMappingPolicy(UClass* InClass);

	void InitClassReplicationInfo(FClassReplicationInfo& Info, UClass* InClass, bool bSpatialize, float ServerMaxTickRate);
	uint8 GetRepPeriodFrame(UClass* InClass);

	//Links classes to map policies.
	TClassMap<EClassRepPolicy> ClassRepPolicies;

	//#TODO : Debug Option to render these out
	float GridCellSize = 6400.0f; //Size of the grid.
	float SpatialBiasX = -WORLD_MAX;
	float SpatialBiasY = -WORLD_MAX;
	bool bDisableSpatialRebuilding = true;
};