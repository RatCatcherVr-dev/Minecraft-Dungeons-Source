#include "network/DungeonsReplicationGraphNode_AlwaysRelevant_ForConnection.h"
#include "DungeonsReplicationGraph.h"

void UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection::GatherActorListsForConnection(const FConnectionGatherActorListParameters& Params)
{
	Super::GatherActorListsForConnection(Params);

	UDungeonsReplicationGraph* RepGraph = CastChecked<UDungeonsReplicationGraph>(GetOuter());

	FPerConnectionActorInfoMap& ConnectionActorInfoMap = Params.ConnectionManager.ActorInfoMap;
	TMap<FName, FActorRepListRefView>& AlwaysRelevantStreamingLevelActors = RepGraph->AlwaysRelevantStreamingLevelActors;

	for (int32 Idx = AlwaysRelevantStreamingLevels.Num() - 1; Idx >= 0; --Idx)
	{
		FName StreamingLevel = AlwaysRelevantStreamingLevels[Idx];
		FActorRepListRefView* ListPtr = AlwaysRelevantStreamingLevelActors.Find(StreamingLevel);

		if (ListPtr == nullptr)
		{
			AlwaysRelevantStreamingLevels.RemoveAtSwap(Idx, 1, false);
			continue;
		}

		FActorRepListRefView& RepList = *ListPtr;
		if (RepList.Num() > 0)
		{
			bool bAllDormant = true;
			for (FActorRepListType Actor : RepList)
			{
				FConnectionReplicationActorInfo& ConnectionActorInfo = ConnectionActorInfoMap.FindOrAdd(Actor);
				if (ConnectionActorInfo.bDormantOnConnection == false)
				{
					bAllDormant = false;
					break;
				}
			}

			if (bAllDormant)
			{
				AlwaysRelevantStreamingLevels.RemoveAtSwap(Idx, 1, false);
			}
			else
			{
				Params.OutGatheredReplicationLists.AddReplicationActorList(RepList);
			}
		}
	}
}

void UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityAdd(FName LevelName, UWorld* LevelWorld)
{
	AlwaysRelevantStreamingLevels.Add(LevelName);
}

void UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityRemove(FName LevelName)
{
	AlwaysRelevantStreamingLevels.Remove(LevelName);
}

void UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection::ResetGameWorldState()
{
	AlwaysRelevantStreamingLevels.Empty();
}