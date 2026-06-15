// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "InterruptableItemInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "AbilitySystemComponent.h"




void AInterruptableItemInstance::BeginPlay() {
	Super::BeginPlay();

	auto player = GetPlayerOwner();
	if (player && (player->IsLocallyControlled() || HasAuthority())) {
		auto* abilitySystem = player->GetAbilitySystemComponent();
		abilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Stunned"))).AddUObject(this, &AInterruptableItemInstance::OnStunned);
		player->GetHealthComponent()->OnDeath.AddUObject(this, &AInterruptableItemInstance::Interrupt_Internal);
	}
}

void AInterruptableItemInstance::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	if (auto player = GetPlayerOwner()) {
		if (auto* abilitySystem = player->GetAbilitySystemComponent()) {
			abilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Stunned"))).RemoveAll(this);
		}
		if (auto* healthComponent = player->GetHealthComponent()) {
			healthComponent->OnDeath.RemoveAll(this);
		}
	}
}

void AInterruptableItemInstance::OnStunned(const FGameplayTag tag, const int32 tagCount) {
	Interrupt_Internal();
}

void AInterruptableItemInstance::Interrupt_Internal() {
	if(IsActive()) Interrupt();
}
