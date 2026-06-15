// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "EmeraldShield.h"
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemGlobals.h>
#include "game/component/WalletComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/actor/character/player/PlayerCharacter.h"

// ----- EMERALD SHIELD GAMEPLAY EFFECT ----- //
UEmeraldShieldGameplayEffect::UEmeraldShieldGameplayEffect() {
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Resistance.Slow"));
}

// ----- EMERALD SHIELD ARMOUR PROPERTY ----- //
void UEmeraldShield::BeginPlay() {
	Super::BeginPlay();
}

void UEmeraldShield::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
}

void UEmeraldShield::OnEmeraldsCollected(int32 amount)
{
	if (GetOwnerRole() == ROLE_Authority) {
		const auto playerCharacter = Cast<APlayerCharacter>(GetOwner());

		if (playerCharacter && playerCharacter->HasAuthority())
		{
			playerCharacter->ApplyInvulnerability(Duration);
			BroadcastArmorPropertyTriggeredEvent();
		}
	}
}

UEmeraldShield::UEmeraldShield() {
	TypeID = EArmorPropertyID::EmeraldShield;
}
