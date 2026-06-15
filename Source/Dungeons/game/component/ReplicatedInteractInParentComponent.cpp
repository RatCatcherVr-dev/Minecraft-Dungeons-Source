
#include "ReplicatedInteractInParentComponent.h"

#include "game/component/ReplicatedInteractableComponent.h"

UReplicatedInteractInParentComponent::UReplicatedInteractInParentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UReplicatedInteractInParentComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UInteractableComponent* Interactable = GetOwner()->FindComponentByClass<UInteractableComponent>())
	{
		Interactable->OnInteracted.AddDynamic(this, &UReplicatedInteractInParentComponent::InteractInParentActor);
	}
}

void UReplicatedInteractInParentComponent::InteractInParentActor(ACharacter* Instigator)
{
	if (const auto* ParentActor = GetOwner()->GetParentActor())
	{
		if (UReplicatedInteractableComponent* ParentInteract = ParentActor->FindComponentByClass<UReplicatedInteractableComponent>())
		{
			ParentInteract->Interacted(Instigator);
		}
	}
}

