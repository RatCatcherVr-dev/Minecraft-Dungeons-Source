// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/component/EquipmentComponent.h"
#include "game/item/ItemSlot.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "AreaHeal.h"
#include <AbilitySystemComponent.h>
#include "game/item/ItemConsoleCommands.h"
#include <IConsoleManager.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include "game/item/instance/HealthPotionInstance.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"

extern TAutoConsoleVariable<int32> CVarDebugDrawItems;

UAreaHeal::UAreaHeal() {
	TypeID = EArmorPropertyID::AreaHeal;
}

void UAreaHeal::BeginPlay(){
	Super::BeginPlay();
	if (const auto ownerEquipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
		auto slots = ownerEquipmentComponent->GetSlotsOfType(ESlotType::HealthPotion);
		if (slots.Num()) {
			auto slot = slots[0];
			if (!slot->OnItemSlotActivationCompleted.Contains(this, "OnHealthPotionActivated")) {
				slot->OnItemSlotActivationCompleted.AddDynamic(this, &UAreaHeal::OnHealthPotionActivated);
			}
		}
	}
}

void UAreaHeal::OnHealthPotionActivated(UItemSlot* slot, bool success) {
	if (success) {

		if (const auto playerOwner = Cast<APlayerCharacter>(GetOwner())) {
			if (CVarDebugDrawItems.GetValueOnGameThread()) DrawDebugSphere(GetWorld(), playerOwner->GetActorLocation(), Radius, 50, FColor::Green, false, 5.0f);

			UAbilitySystemComponent* abilitySystem = playerOwner->GetAbilitySystemComponent();
			abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.ArmorProperty.AreaHeal.Owner"), FGameplayCueParameters());

			FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UHealthPotionGameplayEffect>(abilitySystem, 1.f);
			spec.GetContext().AddInstigator(playerOwner, playerOwner);
			spec.GetContext().AddSourceObject(this);

			if (playerOwner->HasAuthority()) {
				FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
				for (auto characterActor : actorquery::getNearbyActors<ABaseCharacter>(GetOwner(), Radius)) {
					if (playerOwner->IsFriendlyTowards(characterActor)) {
						if(auto TargetAbilitySystem = characterActor->GetAbilitySystemComponent()){
							abilitySystem->ApplyGameplayEffectSpecToTarget(spec, TargetAbilitySystem);
						}
					}
				}
			}
		}
	}
}