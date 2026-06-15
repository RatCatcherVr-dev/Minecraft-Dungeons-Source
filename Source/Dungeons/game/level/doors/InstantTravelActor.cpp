#include "Dungeons.h"
#include "InstantTravelActor.h"
#include "game/component/TeleportComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "game/component/MapPinComponent.h"
#include "game/component/ReplicatedInteractableComponent.h"
#include "game/actor/character/player/BasePlayerController.h"
#include "Engine/LocalPlayer.h"
#include "DungeonsUserManagement.h"
#include "DungeonsGameInstance.h"

AInstantTravelActor::AInstantTravelActor(const FObjectInitializer& objectInitializer) {
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot")));

	Destination = CreateDefaultSubobject<UArrowComponent>(TEXT("Destination"));
	Destination->SetupAttachment(RootComponent);
	Destination->bEditableWhenInherited = true;

	TravelToMeLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("TravelToMePosition"));
	TravelToMeLocation->SetupAttachment(RootComponent);
	TravelToMeLocation->bEditableWhenInherited = true;

	MapPinComponent = CreateDefaultSubobject<UMapPinComponent>(TEXT("MapPinComponent"));

	InteractableComponent = CreateDefaultSubobject<UReplicatedInteractableComponent>(TEXT("Interactable"));
	InteractableComponent->bEditableWhenInherited = true;
}

void AInstantTravelActor::BeginPlay() {
	Super::BeginPlay();

	if (HasAuthority()) {
		InteractableComponent->OnInteractCallback = [&](UReplicatedInteractableComponent&, AActor* interactor) {
			InstantTravelToDestination(interactor);
		};
	}
}

FVector AInstantTravelActor::GetTravelToMeLocation() const {
	return TravelToMeLocation->GetComponentLocation();
}

FRotator AInstantTravelActor::GetTravelToMeRotation() const {
	return TravelToMeLocation->GetComponentRotation();
}

void AInstantTravelActor::SetDestinationLocation(const FVector partnerDoorLocation) const {
	Destination->SetWorldLocation(partnerDoorLocation);
}

void AInstantTravelActor::SetDestinationRotation(const FRotator partnerDoorRotation) const {
	Destination->SetWorldRotation(partnerDoorRotation);
}

void AInstantTravelActor::InstantTravelToDestination(AActor* traveler) {
	if (const auto leadingPlayer = Cast<APlayerCharacter>(traveler)) {
		// #D11.CM	- Teleport to the correct location, taking in to account any local players.

		// Teleport our leading Player
		if (leadingPlayer->GetTeleportComponent()->TeleportToInstantTravel(Destination)) {
			OnInstantTravel(traveler);

			// If the leading player is locally controlled, loop our other local players
			if (leadingPlayer->IsLocallyControlled()) {
				for (auto* localPlayer : GetWorld()->GetGameInstance<UDungeonsGameInstance>()->GetLocalPlayers()) {
					InstantTravelLocalPlayer(localPlayer, leadingPlayer);
				}
			}
		}
	}
}

bool AInstantTravelActor::InstantTravelLocalPlayer(ULocalPlayer* player, APlayerCharacter* leader) {
	if (auto* playerController = Cast<ABasePlayerController>(player->GetPlayerController(GetWorld()))) {
		if (auto* playerCharacter = playerController->GetControlledPlayerCharacter()) {

			if (playerCharacter != leader) {
				if (playerCharacter->GetTeleportComponent()->TeleportToInstantTravel(Destination)) {
					OnInstantTravel(Cast<AActor>(playerCharacter));
					return true;
				}
			}
		}
	}
	return false;
}
