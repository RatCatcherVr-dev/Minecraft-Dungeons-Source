


#include "TogglableItemInstance.h"
#include "UnrealNetwork.h"
#include <AbilitySystemComponent.h>
#include <GameplayPrediction.h>
#include "ClientEventHub.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/EquipmentDisplayComponent.h"


ATogglableItemInstance::ATogglableItemInstance() {
	bHasManualCooldownActivation = true;
	bCanFail = true;
}

void ATogglableItemInstance::ClientToggle_Implementation(bool Enabled) {
	Toggled = Enabled;
	Toggle(FPredictionKey(), Enabled);
}
bool ATogglableItemInstance::ClientToggle_Validate(bool Enabled) {
	return true;
}

void ATogglableItemInstance::ServerToggle_Implementation(FPredictionKey predictionKey, bool Enabled) {
	FScopedPredictionWindow window(GetPlayerOwner()->GetAbilitySystemComponent(), predictionKey);
	Toggled = Enabled;
	BackingToggle = Enabled;
	Toggle(predictionKey, Enabled);
}

bool ATogglableItemInstance::ServerToggle_Validate(FPredictionKey predictionKey, bool Enabled) {
	return true;
}

void ATogglableItemInstance::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	InterruptLocal();
}

bool ATogglableItemInstance::IsBusy() const {
	return Toggled;
}

void ATogglableItemInstance::Activate(const FPredictionKey& predictionKey) {
	Toggled = !Toggled;

	Toggle(predictionKey, Toggled);

	//If the server has triggered the activation and we are not locally controlled, we must tell the client.
	if(HasAuthority() && !GetPlayerOwner()->IsLocallyControlled() && !predictionKey.IsValidKey()) {
		ClientToggle(Toggled);		
	}

	OnActivated();
	OnItemActivationCompleted.Broadcast(true);
	GetPlayerOwner()->GetClientEventHubComponent()->UseItem(GetItemId());
}


void ATogglableItemInstance::OnRep_BackingToggle() {
	check(!HasAuthority());

	auto owner = GetPlayerOwner();

	if (owner && !owner->IsLocallyControlled()) {
		if (BackingToggle) {
			if (ItemActorClass) HandleItemActorSpawn();
			StartAnimation();
		}
		else {
			StopAnimation();
			if (ItemActorClass) HandleItemActorRemoval();
		}
	}
}

void ATogglableItemInstance::StartAnimation() const {
	auto player = GetPlayerOwner();
	if (player)
	{
		if (UAnimMontage* montage = Cast<UAnimMontage>(ActivationAnimation)) {
			player->PlayMontage(montage);
		}
		else if (ActivationAnimation) {
			player->PlayAnimationAsDynamicMontage(ActivationAnimation, ActivationAnimationSlot, 0, 0.2f, 1, -1);
		}
	}
}
void ATogglableItemInstance::StopAnimation() const {
	auto player = GetPlayerOwner();

	if(UAnimMontage* montage = Cast<UAnimMontage>(ActivationAnimation)) {
		player->StopMontage(0.2f, montage);
	} else if(ActivationAnimation) {
		player->StopAnimation(0.2f, ActivationAnimation, ActivationAnimationSlot);
	}
}

void ATogglableItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATogglableItemInstance, BackingToggle);
}

void ATogglableItemInstance::HandleItemActorSpawn() {
	RemoveItemActor();
	SpawnItemActor();

	if (auto* equipmentDisplayComponent = GetOwnerEquipmentDisplayComponent()) {
		equipmentDisplayComponent->HideWeapons();
	}
}

bool ATogglableItemInstance::CanActivate() const {
	return IsToggled() ? bCanBeInterruptedWithUserAction : Super::CanActivate();
}

void ATogglableItemInstance::ForceActivate() {
	if(!IsToggled()) {
		Super::ForceActivate();
	}
}


void ATogglableItemInstance::Toggle(const FPredictionKey& predictionKey, bool Enabled) {
	auto player = GetPlayerOwner();
	
	if (HasAuthority()) {
		BackingToggle = Enabled;
	}

	if(Enabled) {
		if(player->IsLocallyControlled()) {
			if(SharedPassiveCooldown > 0) player->CancelAllActions();
			if(bCanBeInterruptedWithUserAction) {
				player->CancelActions.AddUObject(this, &ATogglableItemInstance::InterruptLocal);
			} else {
				player->SetSharedCooldown(0.f, SharedPassiveCooldown);
			}
		}		
		ActivationSucceeded(predictionKey);
		if(ItemActorClass) HandleItemActorSpawn();
		StartAnimation();
	} else {
		if(player->IsLocallyControlled() && bCanBeInterruptedWithUserAction) {
			player->CancelActions.RemoveAll(this);	
		}
		
		if(ItemActorClass) HandleItemActorRemoval();
		
		StopAnimation();
		OnActivated();
		OnItemActivationCompleted.Broadcast(true);

			
		if (bHasManualCooldownActivation) {
			Cooldown().TriggerCooldown(CalculateCooldown(), predictionKey);		
		}

		if(SharedPassiveCooldown > 0.f && player->IsLocallyControlled() && !bCanBeInterruptedWithUserAction) {
			player->SetSharedCooldown(0.f, SharedPassiveCooldown);
		}
	}

	ForceNetUpdate();
}

void ATogglableItemInstance::InterruptLocal() {
	if(Toggled) {
		auto abilitySystem = GetPlayerOwner()->GetAbilitySystemComponent();
		FScopedPredictionWindow window(abilitySystem);
		Toggled = false;
		BackingToggle = Toggled;
		Toggle(abilitySystem->ScopedPredictionKey, Toggled);
		if(!HasAuthority()) {
			ServerToggle(abilitySystem->ScopedPredictionKey, Toggled);
		}
	}
}

void ATogglableItemInstance::InterruptServer() {
	if(Toggled) {
		auto abilitySystem = GetPlayerOwner()->GetAbilitySystemComponent();
		Toggled = false;
		Toggle(FPredictionKey(), Toggled);
		if(!GetPlayerOwner()->IsLocallyControlled()) {
			ClientToggle(Toggled);
		}
	}
}

void ATogglableItemInstance::Interrupt() {
	if(Toggled) {
		auto abilitySystem = GetPlayerOwner()->GetAbilitySystemComponent();
		Toggled = false;
		Toggle(FPredictionKey(), Toggled);
	}
}
 