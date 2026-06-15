// © 2020 Mojang Synergies AB. TM Microsoft Corporation.

#include "ConduitButton.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "game/item/ItemUtil.h"
#include "item/StorableItem.h"
#include "CommonTypes.h"
#include "DarkConduitInteraction.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/actor/item/ItemEmitterActor.h"

AConduitButton::AConduitButton() {
	RequiresHeldConduitToBeActive = false;
	bReplicates = true;
	Interactable = CreateDefaultSubobject<UReplicatedInteractableComponent>(TEXT("Interactable"));
	Interactable->bOneTimeInteraction = false;
	Interactable->bEditableWhenInherited = true;
	DefaultSceneRootInternal = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	DefaultSceneRootInternal->bEditableWhenInherited = true;
	RootComponent = DefaultSceneRootInternal;
	NetUpdateFrequency = 5.0f;
}

void AConduitButton::BeginPlay()
{
	Super::BeginPlay();
	if (RequiresHeldConduitToBeActive)
	{
		InstanceTracker<AConduitButton>::AddInstance(GetWorld(), this);
		Interactable->DisableInteraction();
	}
}

void AConduitButton::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (RequiresHeldConduitToBeActive)
	{
		InstanceTracker<AConduitButton>::RemoveInstance(GetWorld(), this);
	}
}

void AConduitButton::GiveConduitToCharacter(ACharacter* Character) {
	FInventoryItemData inventoryItemData = FInventoryItemData(game::item::type::Conduit.getId());
	AStorableItem* ConduitStorable = game::item::util::spawnStorableItem(*GetWorld(), Character->GetActorLocation(), inventoryItemData, {}, Character, true);
	ConduitStorable->HandleInteraction(Character);
}

void AConduitButton::DropConduitStorable() {
	auto spawnLocation = GetActorLocation() + FVector::UpVector * 400.0f ;
	auto spawnDirection = FVector::UpVector;
	game::item::util::FSpecifiedStoreCountItemDrop itemConduit(game::item::type::Conduit.getId());
	TArray<game::item::util::FSpecifiedStoreCountItemDrop>	generatedItemData = { itemConduit };
	AItemEmitterActor::SpawnSimpleItemEmitter(GetWorld(), generatedItemData, spawnLocation, nullptr, false, true, false, FItemEmitterActorCompleteDelegate::CreateLambda([spawnDirection](TArray< AStorableItem* > & SpawnedItems) {

		for (auto item : SpawnedItems)
		{
			item->SetReplicates(true);
			item->SetActorTickEnabled(true);			
		}

		game::item::util::SpreadOutItemsInSquareWithDirection(SpawnedItems, spawnDirection, FloatRange(0.0f, 1.0f));
	}));
}

void AConduitButton::CompleteDarkConduit() {
	TArray<ADarkConduitInteraction*>& darkConduitInteractions = InstanceTracker<ADarkConduitInteraction>::GetList(GetWorld());
	for (ADarkConduitInteraction* darkConduitInteraction : darkConduitInteractions) {
		darkConduitInteraction->IsDarkConduitComplete = true;
	}
	
	TArray<APlayerCharacter*> players = InstanceTracker<APlayerCharacter>::GetList(GetWorld());
	for (APlayerCharacter* player : players) {
		player->OnLostConduit();
	}
}

void AConduitButton::OnConduitHeld()
{
	Interactable->EnableInteraction();
}

void AConduitButton::OnConduitDropped()
{
	Interactable->DisableInteraction();
}
