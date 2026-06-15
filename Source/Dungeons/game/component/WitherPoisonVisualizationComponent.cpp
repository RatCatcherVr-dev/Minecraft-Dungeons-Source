// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "WitherPoisonVisualizationComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/effects/WitherGameplayEffect.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include <GameplayEffectTypes.h>
#include "HealthComponent.h"

UWitherPoisonVisualizationComponent::UWitherPoisonVisualizationComponent() {
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = wither::PERIOD;
}

void UWitherPoisonVisualizationComponent::BeginPlay() {
	Super::BeginPlay();

	if (GetCharacterOwner()->IsLocallyControlled() || GetOwnerRole() == ROLE_Authority) {
		auto ownerAbilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
		ownerAbilitySystem->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("StatusEffect.Wither")).AddUObject(this, &UWitherPoisonVisualizationComponent::OnWitherEffectTagCountChanged);
	}
}

ABaseCharacter* UWitherPoisonVisualizationComponent::GetCharacterOwner() const {
	return Cast<ABaseCharacter>(GetOwner());
}

void UWitherPoisonVisualizationComponent::OnWitherEffectTagCountChanged(const FGameplayTag tag, const int32 tagCount) {
	if (tagCount > 0) {
		auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent();
		auto effect = wither::getActiveEffectHandle(abilitySystem);
		abilitySystem->GetActiveEffectEventSet(effect)->OnTimeChanged.AddUObject(this, &UWitherPoisonVisualizationComponent::OnEffectDurationChanged);
		const auto activeEffect = abilitySystem->GetActiveGameplayEffect(effect);
		OnEffectDurationChanged(effect, activeEffect->StartWorldTime, activeEffect->GetDuration());
	}
	else {
		CachedWitherDuration = 0;
		CachedWitherPoisonFraction = 0;
		OnWitherFractionChanged.Broadcast(0, false);
	}

	SetComponentTickEnabled(tagCount > 0);
}

void UWitherPoisonVisualizationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CachedWitherDuration = FMath::Max(CachedWitherDuration - DeltaTime, 0.f);
	CachedWitherPoisonFraction = wither::getWitherFractionForDuration(CachedWitherDuration);
	OnWitherFractionChanged.Broadcast(CachedWitherPoisonFraction, false);
}

void UWitherPoisonVisualizationComponent::OnEffectDurationChanged(FActiveGameplayEffectHandle, float NewStartTime, float NewDuration) {
	CachedWitherDuration = NewStartTime - GetWorld()->GetTimeSeconds() + NewDuration;
	CachedWitherPoisonFraction = wither::getWitherFractionForDuration(CachedWitherDuration);
	OnWitherFractionChanged.Broadcast(CachedWitherPoisonFraction, true);
}