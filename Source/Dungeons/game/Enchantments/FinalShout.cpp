// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "FinalShout.h"
#include "game/item/ItemSlot.h"
#include "game/actor/item/InterruptableItemInstance.h"
#include "game/component/HealthComponent.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/EquipmentComponent.h"
#include <AbilitySystemComponent.h>
#include "../abilities/attributes/HealthAttributeSet.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"


namespace {
	FName FinalShoutEffectDuration(TEXT("FinalShoutEffectDuration"));
	FName FinalShoutTakeDamageMagnitude(TEXT("FinalShoutTakeDamageMagnitude"));
	FName FinalShoutCooldownStatusEffectTagName();
}

UFinalShout::UFinalShout() {
	Effect = UFinalShoutGameplayEffect::StaticClass();
	TypeId = EEnchantmentTypeID::FinalShout;

	LevelMultiplier = [this](int level) -> float {
		return triggerCooldownBase - (level-1) * triggerCooldownPerLevel;
	};
	MultiplierFormatter = valueformat::asEveryRoundedOrdinalSecond;

}

FText UFinalShout::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentage(triggerHealthFractionThreshold)));
}

bool UFinalShout::ReadyToActivate() const {
	return !GetCharacterOwner()->GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Enchantment.FinalShout")));
}


void UFinalShout::OnAfterReceivedDamage(const struct FGameplayEffectModCallbackData &data, FRandomStream& randStream) {
	auto characterOwner = GetCharacterOwner();
	TArray<AInterruptableItemInstance*> interruptableItems;

	if (GetOwnerRole() == ENetRole::ROLE_Authority) {
		if (const auto healthComponent = characterOwner->FindComponentByClass<UHealthComponent>()) {
			//Aware <= is not technically 'less', but it syncs up with the low health warning in the UI - which is a nice bonus.
			if (healthComponent->IsAlive() && (healthComponent->GetCurrentHealthPercentage() <= triggerHealthFractionThreshold) && ReadyToActivate()) {
				if (const auto equipmentComponent = characterOwner->FindComponentByClass<UEquipmentComponent>()) {
					const auto slots = equipmentComponent->GetSlotsOfType(ESlotType::ActivePermanent);

					for (UItemSlot* slot : slots) {
						if (slot) {
							if (AItemInstance* item = slot->GetItem()) {
								if (AInterruptableItemInstance* interruptableItem = Cast<AInterruptableItemInstance>(item)) {
									interruptableItems.Add(interruptableItem);
									interruptableItem->SetAllowInterrupt(false);
								}
								slot->GetItem()->ForceActivate();
							}
						}
					}
					FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
					const float duration = LevelMultiplier(Level);
					UAbilitySystemComponent* abilitySystem = characterOwner->GetAbilitySystemComponent();
					FGameplayEffectSpec spec(Cast<UFinalShoutGameplayEffect>(Effect->GetDefaultObject()), abilitySystem->MakeEffectContext(), Level);
					spec.SetSetByCallerMagnitude(::FinalShoutEffectDuration, duration);
					abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
				}
			}
		}
	}
	for (AInterruptableItemInstance* item : interruptableItems) {
		if (item) {
			item->SetAllowInterrupt(true);
		}
	}
}

UFinalShoutGameplayEffect::UFinalShoutGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FSetByCallerFloat durationMagnitude;
	durationMagnitude.DataName = ::FinalShoutEffectDuration;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;	

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.FinalShout"), 0, 1);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Enchantment.FinalShout")));
}
