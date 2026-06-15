// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "PotionFortification.h"
#include "game/abilities/attributes/HealthAttributeSet.h"
#include <AbilitySystemComponent.h>
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/component/EquipmentComponent.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

namespace {
	FName PotionFortificationEffectDuration(TEXT("PotionFortificationEffectDuration"));
	FName PotionFortificationEffectMagnitude(TEXT("PotionFortificationEffectMagnitude"));
}

UPotionFortification::UPotionFortification() {
	TypeId = EEnchantmentTypeID::PotionFortification;

	LevelMultiplier = [this](int level) -> float {
		return 5.0f + (level - 1) * 2;
	};
	MultiplierFormatter = valueformat::asDurationRoundedSecond;
	Effect = UPotionFortificationGameplayEffect::StaticClass();
}

FText UPotionFortification::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asRelativeMultiplierPercentageChange(1.0f/DamageAbsorbationIncrease)));
}

void UPotionFortification::OnStart() {
	Super::OnStart();
	if (const auto ownerEquipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>()) {
		auto slots = ownerEquipmentComponent->GetSlotsOfType(ESlotType::HealthPotion);
		if (slots.Num()) {
			auto slot = slots[0];
			if (!slot->OnItemSlotActivationCompleted.Contains(this, "OnHealthPotionActivated")) {
				slot->OnItemSlotActivationCompleted.AddDynamic(this, &UPotionFortification::OnHealthPotionActivated);
			}
		}
	}
}

void UPotionFortification::OnHealthPotionActivated(UItemSlot* slot, bool success) {
	if (GetOwner()->HasAuthority() && success) {
		ApplyEffect();
		//BroadcastEnchantmentTriggeredEvent(); Only using status effect for simplicity.
	}
}

void UPotionFortification::ApplyEffect() {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::GELevel);
		const auto ScaledDuration = LevelMultiplier(Level);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, Effect, ScaledDuration);
		spec.SetSetByCallerMagnitude(::PotionFortificationEffectMagnitude, DamageAbsorbationIncrease);
		spec.SetSetByCallerMagnitude(effects::DurationName, ScaledDuration);
		
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

UPotionFortificationGameplayEffect::UPotionFortificationGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;
	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 1;
	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	DurationMagnitude = durationMagnitude;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo defenceInfo;
	defenceInfo.Attribute = UHealthAttributeSet::TakeDamageMultiplierAttribute();

	FSetByCallerFloat defenceMagnitude;
	defenceMagnitude.DataName = ::PotionFortificationEffectMagnitude;

	defenceInfo.ModifierMagnitude = defenceMagnitude;
	defenceInfo.ModifierOp = EGameplayModOp::Type::Division;

	Modifiers.Add(defenceInfo);

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.StatusEffect.Enchantment.PotionFortification")), 0, 1000);

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));
}