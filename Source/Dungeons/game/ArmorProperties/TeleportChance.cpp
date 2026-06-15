// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/util/LocationQuery.h"
#include "AbilitySystemComponent.h"
#include "TeleportChance.h"



UTeleportChance::UTeleportChance() {
	TypeID = EArmorPropertyID::TeleportChance;
}

void UTeleportChance::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {	
	if (randStream.FRandRange(0.0f, 1.0f) < Chance) {
		auto ownerCharacter = Cast<ABaseCharacter>(GetOwner());
		
		const auto& ownerLocation = ownerCharacter->GetActorLocation();
		const auto ownerCapsule = ownerCharacter->FindComponentByClass<UCapsuleComponent>();

		TOptional<FVector> location = locationquery::getRandomLocationAround(*GetWorld(), *ownerCharacter, ownerLocation, TeleportRange);
		if (!location) {
			UE_LOG(LogDungeons, Warning, TEXT("Teleport chance triggered but no new location could be found within the range"));
			return;
		}

		location.GetValue().Z += ownerCapsule->GetScaledCapsuleHalfHeight();

		ownerCharacter->SetActorLocation(location.GetValue());

		if (const auto abilitySystem = ownerCharacter->GetAbilitySystemComponent()) {
			FGameplayCueParameters params;
			params.Instigator = ownerCharacter;

			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.ArmorProperty.TeleportChance"), params);
		}
	}
}
