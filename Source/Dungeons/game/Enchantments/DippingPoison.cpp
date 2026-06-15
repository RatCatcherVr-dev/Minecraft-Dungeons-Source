// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "DippingPoison.h"
#include "game/abilities/attributes/MeleeAttributeSet.h"
#include "../abilities/attributes/MovementAttributeSet.h"
#include "../abilities/effects/GameplayEffectUtil.h"
#include <AbilitySystemComponent.h>
#include "game/actor/character/BaseCharacter.h"
#include "game/item/instance/AItemInstance.h"
#include "game/actor/character/player/PlayerCharacter.h"
#include "game/component/EquipmentComponent.h"
#include "../abilities/attributes/HealthAttributeSet.h"
#include "../abilities/effects/calculations/DamageModCalculations.h"
#include "../abilities/effects/executions/DamageExecutionCalculation.h"
#include "../abilities/effects/calculations/StatusModCalculations.h"

UDippingPoison::UDippingPoison() {
	TypeId = EEnchantmentTypeID::DippingPoison;
	LevelMultiplier = [this](int level) -> float {
		switch (level)
		{
		case 1:
			return arrows_given_1;
		case 2:
			return arrows_given_2;
		case 3:
			return arrows_given_3;
		default:
			return 0;
		}
	};
	MultiplierFormatter = valueformat::asConstant;
}

void UDippingPoison::OnStart() {
	Super::OnStart();
	if (GetOwnerRole() != ROLE_Authority)
		return;

}

void UDippingPoison::OnEnd() {
	Super::OnEnd();
	if (GetOwnerRole() != ROLE_Authority)
		return;
}

void UDippingPoison::OnItemActivated(const AItemInstance* itemActivated, FPredictionKey key)
{
	if (itemActivated->GetItemType().hasTag(ItemTag::HealthPotion)) {
		if (APlayerCharacter* PlayerOwner = Cast<APlayerCharacter>(GetOwner())) {
			const auto& slots = PlayerOwner->GetEquipmentComponent()->GetSlotsOfType(game::item::type::PoisonArrow.slotType());
			for (auto slot : slots) {
				const ItemType& itemType = game::item::type::PoisonArrow;
				if (slot->CanEquip(itemType)) {
					slot->EquipItem(FInventoryItemData(itemType.getId(), 1.f), LevelMultiplier(Level), EEquipmentSource::Crafted);
				}
			}

			if(auto abilitySystem = PlayerOwner->GetAbilitySystemComponent())
			{
				FGameplayCueParameters params;
				params.GameplayEffectLevel = Level;
				params.AbilityLevel = Level;
				abilitySystem->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag("GameplayCue.Trigger.Enchantment.DippingPoison"), params);
			}
		}
	}
}

UPoisonArrowGameplayEffect::UPoisonArrowGameplayEffect(const FObjectInitializer& ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;

	bExecutePeriodicEffectOnApplication = true;
	bRequireModifierSuccessToTriggerCues = true;
	bSuppressStackingCues = true;
	Period = 0.5f;

	FCustomCalculationBasedFloat damageMagnitude;
	damageMagnitude.CalculationClassMagnitude = URangedItemPowerOnlyModDamageCalculation::StaticClass();
	damageMagnitude.Coefficient = damagePerSecond * Period.GetValueAtLevel(1);

	FGameplayModifierInfo info;
	info.Attribute = UHealthAttributeSet::HealthAttribute();
	info.ModifierMagnitude = damageMagnitude;
	Modifiers.Add(info);

	const auto weakDamageTag = FGameplayTag::RequestGameplayTag(TEXT("Damage.Weak"));
	InheritableGameplayEffectTags.AddTag(weakDamageTag);

	FGameplayEffectExecutionDefinition damage;
	damage.CalculationClass = UDamageExecutionCalculation::StaticClass();
	damage.PassedInTags.AddTag(weakDamageTag);
	Executions.Add(std::move(damage));

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Elemental.Poison")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Damage.Medium.Poison")));
	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("StatusEffect.Negative")));
	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Poison"), 0, 1);
}
