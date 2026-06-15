#include "Dungeons.h"
#include "ReplicatedInteractableComponent.h"
#include <Net/UnrealNetwork.h>
#include "InteractableComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "repgraph/Actor_RepSpatializeStatic.h"
#include "../level/doors/InstantTravelActor.h"
#include "game/actor/character/loot/LootActor.h"
#include "game/abilities/cues/DungeonsGameplayCueNotify_Actor.h"
#include "game/actor/interfaces/Activatable.h"

UReplicatedInteractableComponent::UReplicatedInteractableComponent()
	: bIsActivated(false)
	, bActivatableInitialized(false) {
	bReplicates = true;
	bAutoActivate = true;
}

void UReplicatedInteractableComponent::Interact(AActor* instigator) {
	auto owner = GetOwner();

	if (ValidateCallback && !ValidateCallback(*this, instigator)) {
		MulicastOnFail();

		if (OnFailCallback) {
			OnFailCallback(*this, instigator);
		}
	} else {
		if (OnInteractCallback) {
			OnInteractCallback(*this, instigator);
		}
		if (owner->HasAuthority())
		{
			if (owner->NetDormancy > DORM_Awake) {
				owner->FlushNetDormancy();
			}

			if (!bOneTimeInteraction) {
				ServerMakeNonActive();
			}
			if (!bIsActivated) {
				bIsActivated = true;
				OnRep_IsActivated();
			}
		}
	}
}

void UReplicatedInteractableComponent::ClientSafeInteract(ACharacter* InstigatingCharacter) {
	if (!InstigatingCharacter) {
		return;
	}
	
	Interacted(InstigatingCharacter);
}

void UReplicatedInteractableComponent::BeginPlay() {
	Super::BeginPlay();

	ensureMsgf(GetOwner()->GetIsReplicated(), TEXT("Actor %s must be set to replicate, since it uses a UReplicatedInteractableComponent."), *GetOwner()->GetName());
	ensureMsgf(
		GetOwner()->IsA<AActor_RepSpatializeStatic>() ||
		GetOwner()->IsA<APropActor_RepSpatializeStatic>() ||
		GetOwner()->IsA<AActor_RepSpatializeDynamic>() ||
		GetOwner()->IsA<APropActor_RepSpatializeDynamic>() ||
		GetOwner()->IsA<AActor_RepSpatializeDormancy>() ||
		GetOwner()->IsA<APropActor_RepSpatializeDormancy>() ||
		GetOwner()->IsA<AActor_RepAlways>() ||
		GetOwner()->IsA<APropActor_RepAlways>() ||
	    GetOwner()->IsA<AInstantTravelActor>() ||
		GetOwner()->IsA<ALootActor>(),
		TEXT("UReplicatedInteractableComponents on '%s' need to be attached to an actor which inherits from one of the Rep policy classes."),
		*GetOwner()->GetName()
	);


	if (bIsActivated) {
		OnRep_IsActivated();
	}
}

void UReplicatedInteractableComponent::initActivatable()
{
	if (!bActivatableInitialized)
	{
		bActivatableInitialized = true;
		OnRep_ActivatableInitialized();
	}
}

void UReplicatedInteractableComponent::Interacted(ACharacter* character) {
	Super::Interacted(character);
	
	if (auto controller = Cast<ABasePlayerController>(character->GetController())) {
		controller->ServerInteract(this, character);
	}
}

void UReplicatedInteractableComponent::SetLastInstigator(ACharacter* instigator)
{
	LastInstigator = instigator;

	if (GetOwner()->HasAuthority()) {
		GetOwner()->ForceNetUpdate();
	}
}

void UReplicatedInteractableComponent::EnableInteraction() {
	Super::EnableInteraction();
	ServerMakeNonActive();
}

void UReplicatedInteractableComponent::ServerMakeNonActive() {
	if (GetOwner()->HasAuthority()) {
		bIsActivated = false;
	}
}

void UReplicatedInteractableComponent::OnRep_IsActivated() {
	if (bIsActivated) {
		OnReplicatedInteract.Broadcast();

		if (bOneTimeInteraction) {
			DisableInteraction();
		}
	}
}

void UReplicatedInteractableComponent::OnRep_ActivatableInitialized() {
	if (GetOwner()->GetClass()->ImplementsInterface(UActivatable::StaticClass()))
	{
		IActivatable::Execute_OnActivated(GetOwner());
	}
}

void UReplicatedInteractableComponent::MulicastOnFail_Implementation() {
	OnReplicatedFail.Broadcast();
}

void UReplicatedInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UReplicatedInteractableComponent, bIsActivated, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UReplicatedInteractableComponent, bActivatableInitialized, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UReplicatedInteractableComponent, LastInstigator, COND_None, REPNOTIFY_Always);
}
