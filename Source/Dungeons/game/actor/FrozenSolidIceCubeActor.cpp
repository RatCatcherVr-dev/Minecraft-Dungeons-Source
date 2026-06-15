#include "Dungeons.h"
#include "FrozenSolidIceCubeActor.h"
#include "AbilitySystemComponent.h"
#include "UnrealNetwork.h"
#include "character/player/PlayerCharacter.h"
#include "game/abilities/attributes/FrozenSolidIceCubeAttributeSet.h"

// #D11.CM

AFrozenSolidIceCubeActor::AFrozenSolidIceCubeActor() {
	SetReplicates(true);

	bCollideWhenPlacing = false;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMeshComponent);

	ReplicatedInteractable = CreateDefaultSubobject<UReplicatedInteractableComponent>(TEXT("Interactable"));
	ReplicatedInteractable->AttachTo(StaticMeshComponent);
	ReplicatedInteractable->SetType(EClickyEnum::CE_IceBlock);
	ReplicatedInteractable->OnInteractCallback = [this](const UReplicatedInteractableComponent& component, AActor* instigator) {
		OnPlayerInteraction(instigator);
	};
}

void AFrozenSolidIceCubeActor::BeginPlay() {
	Super::BeginPlay();
	if (auto world = GetWorld()) {
		world->GetTimerManager().SetTimer(BounceBlockerTimerHandle, this, &AFrozenSolidIceCubeActor::EnableBounce, InteractionCountdown);
	}
}

void AFrozenSolidIceCubeActor::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	if (auto world = GetWorld()) {
		world->GetTimerManager().ClearAllTimersForObject(this);
	}
}

void AFrozenSolidIceCubeActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFrozenSolidIceCubeActor, interactCount);
	DOREPLIFETIME(AFrozenSolidIceCubeActor, ReplicatedInteractable);
}

void AFrozenSolidIceCubeActor::OnPlayerInteraction(AActor* interactor) {
	if (IsPendingKill())
		return;

	// Update interact counter
	float OwnerInteractionContributionAttributeValue = 0, OtherInteractionContributionAttributeValue = 0;
	if (UAbilitySystemComponent* AbilitySystem = GetOwner()->FindComponentByClass<UAbilitySystemComponent>())
	{
		OwnerInteractionContributionAttributeValue = AbilitySystem->GetNumericAttribute(UFrozenSolidIceCubeAttributeSet::OwnerInteractionMagnitudeAttribute());
		OtherInteractionContributionAttributeValue = AbilitySystem->GetNumericAttribute(UFrozenSolidIceCubeAttributeSet::OtherInteractionMagnitudeAttribute());
	}


	interactCount += interactor == GetOwner() ? OwnerInteractionContribution * OwnerInteractionContributionAttributeValue : OtherInteractionContribution * OtherInteractionContributionAttributeValue;

	
	//Remove our frozen solid effect
	if (interactCount >= InteractionLimit) {
		DestroyIcecube();
		return;
	}
	else {
		// Bounce our owner if we can
		ApplyBounceToOwner();

		// Broadcast that we've been interacted with
		OnPlayerInteracted.Broadcast();
	}
}

void AFrozenSolidIceCubeActor::ApplyBounceToOwner() const {
	if (!BounceEnabled) {
		return;
	}

	if (auto* movement = GetOwner()->FindComponentByClass<UCharacterMovementComponent>()) {
		if (movement->MovementMode != EMovementMode::MOVE_Falling) {
			if (auto* character = Cast<ABaseCharacter>(GetOwner())) {
				// Only launch our owner if we're not already falling
				character->LaunchCharacter(FVector{ 0, 0, InteractionLaunchHeight }, false, false);
			}
		}
	}
}

bool AFrozenSolidIceCubeActor::RemoveFrozenSolidEffectFromActor(AActor* effected) {
	if (UAbilitySystemComponent* abilitySystem = effected->FindComponentByClass<UAbilitySystemComponent>()) {
		FGameplayTagContainer tag;
		tag.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Immobile.FrozenSolid"));
		abilitySystem->RemoveActiveEffectsWithTags(tag);

		return true;
	}
	
	return false;
}

void AFrozenSolidIceCubeActor::DestroyIcecube() {
	if (RemoveFrozenSolidEffectFromActor(GetOwner())) {
		ReplicatedInteractable->DisableInteraction();
		ReplicatedInteractable->RemoveFromRoot();
		ReplicatedInteractable->DestroyComponent();

		Destroy();
	}
}
