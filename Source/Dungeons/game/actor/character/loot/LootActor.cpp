#include "Dungeons.h"
#include "LootActor.h"
#include "DungeonsGameInstance.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/MapPinComponent.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "game/component/LootUnlockerComponent.h"
#include <Animation/AnimComposite.h>
#include <Net/UnrealNetwork.h>
#include "game/ForceFeedback.h"
#include "Assets/DungeonsAssetManager.h"

//// InGame-Chest
ALootActor::ALootActor() : DropVelocityMinMultiplier(1.0), DropVelocityMaxMultiplier(1.0), DropConeRadiusMultiplier(1.0)
{
	SetReplicates(true);
	bAlwaysRelevant = true;
	NetUpdateFrequency = 2.0f;
	MinNetUpdateFrequency = 1.0f;


	bLootUnlocked = false;
	RootSceneComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
	RootSceneComponent->SetMobility(EComponentMobility::Static);
	MapPinComponent = CreateDefaultSubobject<UMapPinComponent>(TEXT("MapPinComponent"));

	SetRootComponent(RootSceneComponent);

	InteractableComp = CreateDefaultSubobject<UReplicatedInteractableComponent>(TEXT("Interactable"));
	InteractableComp->SetType(EClickyEnum::CE_Interactable);
	InteractableComp->OnInteractCallback = [this](const UReplicatedInteractableComponent& component, AActor* instigator)
	{
		_onInteract(instigator);
	};
}

void ALootActor::BeginPlay()
{
	Super::BeginPlay();
	InstanceTracker< ALootActor >::AddInstance(GetWorld(), this);

	if (IsOpen() && MeshComp)
	{
		MeshComp->PlayAnimation(unlockingAnim, false);
	}
}

void ALootActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	InstanceTracker< ALootActor >::RemoveInstance(GetWorld(), this);
}

void ALootActor::Open(APlayerCharacter* playerCharacter)
{
	const auto SpawnLocation = GetActorLocation() + AboveGroundOffset;
	if (OnOpenLoot.IsBound()) {
		OnOpenLoot.Broadcast(SpawnLocation, playerCharacter, this);
	}
	OnRep_ActivatedLoot();
	bLootUnlocked = true;
}

void ALootActor::LazyLoadMeshComponent()
{
	if (auto gameInstance = Cast<UDungeonsGameInstance>(GetWorld()->GetGameInstance()))
	{
		TArray<USceneComponent*> rootChildrenComponents;
		RootSceneComponent->GetChildrenComponents(true, rootChildrenComponents);
		for (int index = 0; index < rootChildrenComponents.Num(); ++index)
		{
			if (USkeletalMeshComponent* skelMeshComp = Cast<USkeletalMeshComponent>(rootChildrenComponents[index]))
			{
				MeshComp = skelMeshComp;
				break;
			}
		}
	}
}

void ALootActor::OnRep_ActivatedLoot()
{
	if (MeshComp == nullptr)
	{
		LazyLoadMeshComponent();
	}

	if (MeshComp)
	{
		MeshComp->PlayAnimation(unlockingAnim, false);
	}

	if (OnActivatedLoot.IsBound())
	{
		OnActivatedLoot.Broadcast();
	}	
	InteractableComp->DisableInteraction();

	RemoveAndDestroyComponent(MapPinComponent);
	MapPinComponent = nullptr;
}

void ALootActor::_onInteract(AActor* interactActor)
{

	//Only player characters can interact
	if (auto playerActor = static_cast<APlayerCharacter*>(interactActor))
	{
		//player controller is the actor with the loot unlocker
		if (auto playerController = playerActor->GetController())
		{
			//LootUnlockerComponent handles the unlocking process between server and client
			if (auto lootUnlocker = Cast<ULootUnlockerComponent>(
				playerController->GetComponentByClass(ULootUnlockerComponent::StaticClass())))
			{
				lootUnlocker->ServerRequestOpen(this);
			}
		}
	}
}

void ALootActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALootActor, bLootUnlocked);
	DOREPLIFETIME(ALootActor, InteractableComp);
}

void ALootActor::RemoveAndDestroyComponent(UActorComponent* Component)
{
	if (Component)
	{
		Component->RemoveFromRoot();
		Component->DestroyComponent();
	}
}