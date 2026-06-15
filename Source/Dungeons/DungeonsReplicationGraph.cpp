#include "DungeonsReplicationGraph.h"

#include "game/actor/character/player/PlayerCharacter.h"
#include "ConstructorHelpers.h"
#include "game/actor/Dimmer.h"
#include "game/actor/EffectsActor.h"
#include "ui/MissionProgressHandler.h"
#include "game/avatar/AvatarInfo.h"
#include "game/avatar/PlayerAvatarComponent.h"
#include "DungeonsGameState.h"
#include "LevelSequenceActor.h"
#include "DungeonsReplicationGraphDebug.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "network/DungeonsReplicationGraphNode_AlwaysRelevant_ForConnection.h"
#include "game/level/doors/InstantTravelActor.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "GameplayCueNotify_Actor.h"
#include "game/actor/item/StorableItem.h"
#include "game/level/InstancedMeshBase.h"
#include "game/component/ReplicateLongDistanceComponent.h"

DEFINE_LOG_CATEGORY(LogDungeonsReplicationGraph);

namespace repGraph {
	bool isDistanceRelevant(const AActor* Actor)
	{
		return !(Actor->bAlwaysRelevant || Actor->bOnlyRelevantToOwner || Actor->bNetUseOwnerRelevancy);
	};
}

void UDungeonsReplicationGraph::ResetGameWorldState()
{
	Super::ResetGameWorldState();
	AlwaysRelevantStreamingLevelActors.Empty();

	for (auto& ConnectionList : { Connections, PendingConnections })
	{
		for (UNetReplicationGraphConnection* Connection : ConnectionList)
		{
			for (UReplicationGraphNode* ConnectionNode : Connection->GetConnectionGraphNodes())
			{
				UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection* Node = Cast<UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection>(ConnectionNode);
				if (Node != nullptr)
				{
					Node->ResetGameWorldState();
				}
			}
		}
	}
}

void UDungeonsReplicationGraph::InitConnectionGraphNodes(UNetReplicationGraphConnection* ConnectionManager)
{
	UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection* Node = CreateNewNode<UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection>();
	ConnectionManager->OnClientVisibleLevelNameAdd.AddUObject(Node, &UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityAdd);
	ConnectionManager->OnClientVisibleLevelNameRemove.AddUObject(Node, &UDungeonsReplicationGraphNode_AlwaysRelevant_ForConnection::OnClientLevelVisibilityRemove);
}

void UDungeonsReplicationGraph::InitGlobalActorClassSettings()
{
	Super::InitGlobalActorClassSettings();

	ClassRepPolicies.Reset();

	auto SetRule = [&](UClass* InClass, EClassRepPolicy Mapping) 
	{ 
		check(InClass);
		if (InClass == nullptr)
		{
			UE_LOG(LogDungeonsReplicationGraph, Fatal, TEXT("A null class was set to be replicated.'"));
		}
		ClassRepPolicies.Set(InClass, Mapping); 
	};

	SetRule(AReplicationGraphDebugActor::StaticClass(), EClassRepPolicy::NotRouted);
	SetRule(AServerStatReplicator::StaticClass(), EClassRepPolicy::NotRouted);
	SetRule(APlayerController::StaticClass(), EClassRepPolicy::NotRouted);
	SetRule(ALevelSequenceActor::StaticClass(), EClassRepPolicy::NotRouted);

	SetRule(APlayerCharacter::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(AAvatarInfo::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(UPlayerAvatarComponent::StaticClass(), EClassRepPolicy::RelevantAllConnections);

	SetRule(ADimmer::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(AEffectsActor::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(AMapStatActor::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(AMissionProgressHandler::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(ADungeonsGameState::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(AMobCharacter::StaticClass(), EClassRepPolicy::Spatialize_Dormancy);
	SetRule(AStorableItem::StaticClass(), EClassRepPolicy::Spatialize_Static);
	SetRule(AInstantTravelActor::StaticClass(), EClassRepPolicy::Spatialize_Static);
	SetRule(AStaticMeshActor::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(AInstancedMeshBase::StaticClass(), EClassRepPolicy::RelevantAllConnections);

	//Cursom BP parent classes below.
	SetRule(AActor_RepSpatializeStatic::StaticClass(), EClassRepPolicy::Spatialize_Static);
	SetRule(APropActor_RepSpatializeStatic::StaticClass(), EClassRepPolicy::Spatialize_Static);

	SetRule(AActor_RepSpatializeDynamic::StaticClass(), EClassRepPolicy::Spatialize_Dynamic);
	SetRule(APropActor_RepSpatializeDynamic::StaticClass(), EClassRepPolicy::Spatialize_Dynamic);

	SetRule(AActor_RepSpatializeDormancy::StaticClass(), EClassRepPolicy::Spatialize_Dormancy);
	SetRule(APropActor_RepSpatializeDormancy::StaticClass(), EClassRepPolicy::Spatialize_Dormancy);
	SetRule(AGameplayCueNotify_Actor::StaticClass(), EClassRepPolicy::Spatialize_Dormancy);

	SetRule(AActor_RepAlways::StaticClass(), EClassRepPolicy::RelevantAllConnections);
	SetRule(APropActor_RepAlways::StaticClass(), EClassRepPolicy::RelevantAllConnections);

	SetRule(AActor_RepNotRouted::StaticClass(), EClassRepPolicy::NotRouted);
	SetRule(APropActor_RepNotRouted::StaticClass(), EClassRepPolicy::NotRouted);


	//Determine which classes need to be added to the correct grid types.
	TArray<UClass*> ReplicatedClasses;
	for (TObjectIterator<UClass> Itr; Itr; ++Itr)
	{
		UClass* Class = *Itr;
		AActor* ActorCDO = Cast<AActor>(Class->GetDefaultObject());

		//Check to see if the actor has a ReplicatedInteractable component. If so, get them added.
		if (ActorCDO)
		{
			auto interactable = ActorCDO->FindComponentByClass(UReplicatedInteractableComponent::StaticClass());

			if (interactable && !ClassRepPolicies.Contains(Class, true))
			{
				UE_LOG(LogDungeonsReplicationGraph, Log, TEXT("ReplicatedInteractable Class '%s' Added to Spatialize Static"), *Class->GetName());
				ReplicatedClasses.Add(Class);
				SetRule(Class, EClassRepPolicy::Spatialize_Static);
				continue;
			}
		}


		//No point in continuing if the actor isnt even replicated.
		if (!ActorCDO || !ActorCDO->GetIsReplicated())
		{
			continue;
		}

		//Filter out skeletal and reinstanced classes.
		FString ClassName = Class->GetName();
		if (ClassName.StartsWith("SKEL_") || ClassName.StartsWith("REINST_"))
		{
			continue;
		}

		ReplicatedClasses.Add(Class);	

		// Check if class has been mapped to a policy.
		if (ClassRepPolicies.Contains(Class, false))
		{
			continue;
		}

		auto ShouldSpatialize = [](const AActor* Actor)
		{
			return Actor->GetIsReplicated() && repGraph::isDistanceRelevant(Actor);
		};

		if (ShouldSpatialize(ActorCDO))
		{
			SetRule(Class, EClassRepPolicy::Spatialize_Dynamic);
			UE_LOG(LogDungeonsReplicationGraph, Log, TEXT("Class '%s' Added to Spatialize Dynamic"), *Class->GetName());
		}
		else if (ActorCDO->bAlwaysRelevant && !ActorCDO->bOnlyRelevantToOwner)
		{
			UE_LOG(LogDungeonsReplicationGraph, Log, TEXT("Class '%s' Added to Relevant All Connections"), *Class->GetName());
			SetRule(Class, EClassRepPolicy::RelevantAllConnections);
		}
	}

	//Set Replication info for specific classes.
	TArray<UClass*> ExplicitlySetClasses;

	auto SetClassInfo = [&](UClass* InClass, FClassReplicationInfo RepInfo)
	{
		RepInfo.ReplicationPeriodFrame = GetRepPeriodFrame(InClass);
		GlobalActorReplicationInfoMap.SetClassInfo(InClass, RepInfo);
		ExplicitlySetClasses.Add(InClass);
	};

	//Configure replication settings for our custom BP parent classes. 
	FClassReplicationInfo RepAlwaysInfo;
	SetClassInfo(AActor_RepAlways::StaticClass(), RepAlwaysInfo);
	SetClassInfo(APropActor_RepAlways::StaticClass(), RepAlwaysInfo);

	FClassReplicationInfo RepSpatializeInfo;
	RepSpatializeInfo.CullDistanceSquared = GridCellSize * GridCellSize;
	SetClassInfo(AActor_RepSpatializeStatic::StaticClass(), RepSpatializeInfo);
	SetClassInfo(APropActor_RepSpatializeStatic::StaticClass(), RepSpatializeInfo);
	SetClassInfo(AActor_RepSpatializeDynamic::StaticClass(), RepSpatializeInfo);
	SetClassInfo(APropActor_RepSpatializeDynamic::StaticClass(), RepSpatializeInfo);
	SetClassInfo(AActor_RepSpatializeDormancy::StaticClass(), RepSpatializeInfo);
	SetClassInfo(APropActor_RepSpatializeDormancy::StaticClass(), RepSpatializeInfo);
	SetClassInfo(AActor_RepNotRouted::StaticClass(), RepSpatializeInfo);
	SetClassInfo(APropActor_RepNotRouted::StaticClass(), RepSpatializeInfo);
	SetClassInfo(AGameplayCueNotify_Actor::StaticClass(), RepSpatializeInfo);
	SetClassInfo(AStorableItem::StaticClass(), RepSpatializeInfo);
	SetClassInfo(AInstantTravelActor::StaticClass(), RepSpatializeInfo);
	SetClassInfo(UGameplayEffect::StaticClass(), RepSpatializeInfo);

	for (UClass* ReplicatedClass : ReplicatedClasses)
	{ 
		if (ExplicitlySetClasses.FindByPredicate([&](const UClass* InClass) { return ReplicatedClass->IsChildOf(InClass); }) != nullptr)
		{
			continue;
		}

		bool bSpatialize = IsSpatialized(ClassRepPolicies.GetChecked(ReplicatedClass));
		FClassReplicationInfo ClassRepInfo;
		InitClassReplicationInfo(ClassRepInfo, ReplicatedClass, bSpatialize, NetDriver->NetServerMaxTickRate);
		GlobalActorReplicationInfoMap.SetClassInfo(ReplicatedClass, ClassRepInfo);
		FString name = ReplicatedClass->GetName();
		UE_LOG(LogDungeonsReplicationGraph, Log, TEXT("Class '%s' Has not been explicitly set.."), *ReplicatedClass->GetName());
	}
}

void UDungeonsReplicationGraph::InitGlobalGraphNodes()
{
	//Preallocate some lists, as we use them a lot. This might be overkill, and needs investigated.
	PreAllocateRepList(3, 12);
	PreAllocateRepList(6, 12);
	PreAllocateRepList(128, 64);
	PreAllocateRepList(512, 16);

	//Configure the 2D Grid.
	GridNode = CreateNewNode < UReplicationGraphNode_GridSpatialization2D>();
	GridNode->CellSize = GridCellSize;
	GridNode->SpatialBias = FVector2D(SpatialBiasX, SpatialBiasY);

	//Blacklist all actors from resizing the grid.
	if (bDisableSpatialRebuilding)
	{
		GridNode->AddSpatialRebuildBlacklistClass(AActor::StaticClass());
	}

	AddGlobalGraphNode(GridNode);

	//Configure always relevant actors. (Currently only used for players.)
	AlwaysReleventNode = CreateNewNode<UReplicationGraphNode_ActorList>();
	AddGlobalGraphNode(AlwaysReleventNode);
}

void UDungeonsReplicationGraph::RouteAddNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo, FGlobalActorReplicationInfo& GlobalInfo)
{
	switch (GetMappingPolicy(ActorInfo.Class))
	{
	case EClassRepPolicy::RelevantAllConnections:
		AlwaysReleventNode->NotifyAddNetworkActor(ActorInfo);
		break;
	case EClassRepPolicy::Spatialize_Static:
		check(GlobalInfo.Settings.CullDistanceSquared > 0 && "Classes need a proper cull distance if they're added to the grid");
		GridNode->AddActor_Static(ActorInfo, GlobalInfo);
		break;
	case EClassRepPolicy::Spatialize_Dynamic:
		check(GlobalInfo.Settings.CullDistanceSquared > 0 && "Classes need a proper cull distance if they're added to the grid");
		GridNode->AddActor_Dynamic(ActorInfo, GlobalInfo);
		break;
	case EClassRepPolicy::Spatialize_Dormancy:
		check(GlobalInfo.Settings.CullDistanceSquared > 0 && "Classes need a proper cull distance if they're added to the grid");
		GridNode->AddActor_Dormancy(ActorInfo, GlobalInfo);
		break;
	case EClassRepPolicy::NotRouted:
		break;
	default:
		//checkf(false, TEXT("Class '%s' Was Not Configured."), *ActorInfo.Class->GetName());
		UE_LOG(LogDungeonsReplicationGraph, Log, TEXT("Class '%s' Was Not Configured."), *ActorInfo.Class->GetName());
		break;
	}
}

void UDungeonsReplicationGraph::RouteRemoveNetworkActorToNodes(const FNewReplicatedActorInfo& ActorInfo)
{
	switch (GetMappingPolicy(ActorInfo.Class))
	{
	case EClassRepPolicy::RelevantAllConnections:
		AlwaysReleventNode->NotifyRemoveNetworkActor(ActorInfo);
		break;
	case EClassRepPolicy::Spatialize_Static:
		GridNode->RemoveActor_Static(ActorInfo);
		break;
	case EClassRepPolicy::Spatialize_Dynamic:
		GridNode->RemoveActor_Dynamic(ActorInfo);
		break;
	case EClassRepPolicy::Spatialize_Dormancy:
		GridNode->RemoveActor_Dormancy(ActorInfo);
		break;
	default:
		break;
	}
}

void UDungeonsReplicationGraph::AddNetworkActor(AActor* Actor) {
	auto ActorComponent = Actor->FindComponentByClass(UReplicateLongDistanceComponent::StaticClass());

	if (ActorComponent && !GlobalActorReplicationInfoMap.Find(Actor)) {
		FClassReplicationInfo ClassRepInfo;
		ClassRepInfo.CullDistanceSquared = Cast<UReplicateLongDistanceComponent>(ActorComponent)->CullDistanceSquared;
		ClassRepInfo.ReplicationPeriodFrame = GetRepPeriodFrame(Actor->GetClass());
		GlobalActorReplicationInfoMap.SetClassInfo(Actor->GetClass(), ClassRepInfo);
	}

	Super::AddNetworkActor(Actor);
}

void UDungeonsReplicationGraph::InitClassReplicationInfo(FClassReplicationInfo& Info, UClass* InClass, bool bSpatialize, float ServerMaxTickRate)
{
	if (AActor* CDO = Cast<AActor>(InClass->GetDefaultObject()))
	{
		if (bSpatialize)
		{
			Info.CullDistanceSquared = CDO->NetCullDistanceSquared;
		}

		if (!repGraph::isDistanceRelevant(CDO)) {
			Info.DistancePriorityScale = 0;
		}

		Info.ReplicationPeriodFrame = FMath::Max<uint32>(FMath::RoundToFloat(ServerMaxTickRate / CDO->NetUpdateFrequency), 1);
	}
}

uint8 UDungeonsReplicationGraph::GetRepPeriodFrame(UClass* InClass)
{
	const uint8 DefaultReplicationFrame = 3;

	switch (GetMappingPolicy(InClass))
	{
	case EClassRepPolicy::RelevantAllConnections:
		return DefaultReplicationFrame - 1;
	case EClassRepPolicy::Spatialize_Dynamic:
		return DefaultReplicationFrame;
	case EClassRepPolicy::Spatialize_Static:
		return DefaultReplicationFrame + 1;
	case EClassRepPolicy::Spatialize_Dormancy:
	case EClassRepPolicy::NotRouted:
	case EClassRepPolicy::NotConfigured:
	default:
		if (AActor* CDO = Cast<AActor>(InClass->GetDefaultObject())) {
			return FMath::Max<uint8>(FMath::RoundToFloat(NetDriver->NetServerMaxTickRate / CDO->NetUpdateFrequency), DefaultReplicationFrame);
		}
		else {
			return DefaultReplicationFrame;
		}
	}
	checkNoEntry();
	return 0;
}

EClassRepPolicy UDungeonsReplicationGraph::GetMappingPolicy(UClass* InClass)
{
	return ClassRepPolicies.Get(InClass) != nullptr ? *ClassRepPolicies.Get(InClass) : EClassRepPolicy::NotConfigured;
}