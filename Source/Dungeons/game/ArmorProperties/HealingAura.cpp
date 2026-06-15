// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include <GameplayEffect.h>
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "HealingAura.h"
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>
#include <AbilitySystemComponent.h>
#include "game/component/AreaBuffComponent.h"
#include <GameplayTagContainer.h>
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/actor/character/player/PlayerCharacter.h"

UHealingAura::UHealingAura() {
	TypeID = EArmorPropertyID::HealingAura;
}

void UHealingAura::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() != ROLE_Authority) return;

	if (const auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
		OwnerAreaBuffComponent = NewObject<UAreaBuffComponent>(playerOwner, FName("Healing aura buff component"));
		OwnerAreaBuffComponent->Attributes.Emplace(UHealthAttributeSet::ReceiveHealingMultiplierAttribute(), Amount);
		OwnerAreaBuffComponent->Effect = HealingAuraEffectToApply;
		OwnerAreaBuffComponent->AreaSize = Radius;
		OwnerAreaBuffComponent->RegisterComponent();
	}
}

void UHealingAura::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	if (OwnerAreaBuffComponent.IsValid()) {
		OwnerAreaBuffComponent->DestroyComponent();
	}
}

UHealingAuraGameplayEffect::UHealingAuraGameplayEffect(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.ArmorProperty.HealingAura"), 0, 1);
	effects::AddSetByCallerAttribute(*this, UHealthAttributeSet::ReceiveHealingMultiplierAttribute(), EGameplayModOp::Additive);
}
