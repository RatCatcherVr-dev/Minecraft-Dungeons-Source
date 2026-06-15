


#include "CustomMoverItemInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"

void ACustomMoverItemInstance::OnCustomMoverActivated(ECustomMoverSlot slot) {
	if(Slot == slot) {
		OnMoverTriggered();
	}
}

void ACustomMoverItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACustomMoverItemInstance, Slot);
}

void ACustomMoverItemInstance::BeginPlay() {
	Super::BeginPlay();
	auto player = GetPlayerOwner();
	
	if(!HasAuthority() || !player)  return;

	auto playerMovement = Cast<UPlayerCharacterMovementComponent>(player->GetMovementComponent());
	Slot = playerMovement->RegisterCustomMover(CustomMover);
	OnRep_Slot();
}
void ACustomMoverItemInstance::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);
	auto player = GetPlayerOwner();
	
	if(!HasAuthority() || !player)  return;

	auto playerMovement = Cast<UPlayerCharacterMovementComponent>(player->GetMovementComponent());
	playerMovement->RemoveMoverFromSlot(Slot);
	playerMovement->OnCustomMoveExecuted.RemoveAll(this);
}

void ACustomMoverItemInstance::Activate(const FPredictionKey& predictionKey) {
	if(auto player = GetPlayerOwner()) {
		auto playerMovement = Cast<UPlayerCharacterMovementComponent>(player->GetMovementComponent());
		if(player->IsLocallyControlled() && playerMovement && Slot != ECustomMoverSlot::INVALID) {
			playerMovement->SetWantsToCustomMove(Slot);
			
		}
	}
	Super::Activate(predictionKey);
}

void ACustomMoverItemInstance::OnRep_Slot() {
	if(Slot != ECustomMoverSlot::INVALID) {
		if(auto player = GetPlayerOwner()) {
			if(auto playerMovement = Cast<UPlayerCharacterMovementComponent>(player->GetMovementComponent())) {
				playerMovement->OnCustomMoveExecuted.AddUObject(this, &ACustomMoverItemInstance::OnCustomMoverActivated);		
			}	
		}
	}
}

void ACustomMoverItemInstance::OnMoverTriggered() {}
