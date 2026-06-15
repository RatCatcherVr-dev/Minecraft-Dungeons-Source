#pragma once
#include "ReplicationGraph.h"
#include "ReplicationGraphTypes.h"
#include "DungeonsReplicationGraphNode_AlwaysRelevant_ForConnection.generated.h"

UCLASS()
class UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection : public UReplicationGraphNode_AlwaysRelevant_ForConnection
{
public:

	GENERATED_BODY()

	virtual void GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params) override;

	void OnClientLevelVisibilityAdd(FName LevelName, UWorld* LevelWorld);
	void OnClientLevelVisibilityRemove(FName LevelName);

	void ResetGameWorldState();

protected:
	TArray<FName, TInlineAllocator<64>> AlwaysRelevantStreamingLevels;
};