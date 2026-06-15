#include "Dungeons.h"
#include "DoorComponent.h"
#include <UnrealNetwork.h>

UDoorComponent::UDoorComponent()
	: IsOpen(false), StartOpen(false) {
	bReplicates = true;
}

void UDoorComponent::Open(bool instant) {
	OnOpen.Broadcast(instant);
	UpdateIsOpenOnServer(true);
}

void UDoorComponent::Close(bool instant) {
	OnClose.Broadcast(instant);
	UpdateIsOpenOnServer(false);
}

void UDoorComponent::BeginPlay() {
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority) {
		IsOpen = StartOpen;
	}

	if (IsOpen) {
		Open(true);
	}
}

void UDoorComponent::OnRep_IsOpen() {
	OnReppedVar();
}

void UDoorComponent::OnRep_StartOpen() {
	OnReppedVar();
}

void UDoorComponent::OnReppedVar() {
	if (HasBegunPlay() ? IsOpen : StartOpen) {
		Open(StartOpen);
	}
	else {
		Close(!HasBegunPlay());
	}
}

void UDoorComponent::UpdateIsOpenOnServer(bool isOpen) {
	if (GetOwner()->HasAuthority()) {
		if (HasBegunPlay()) {
			IsOpen = isOpen;
		}
		else {
			StartOpen = isOpen;
		}
	}
}

void UDoorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UDoorComponent, IsOpen);
	DOREPLIFETIME(UDoorComponent, StartOpen);
}
