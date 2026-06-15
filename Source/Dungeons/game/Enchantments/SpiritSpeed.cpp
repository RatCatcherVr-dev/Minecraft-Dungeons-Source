// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "SpiritSpeed.h"
#include "game/abilities/attributes/MovementAttributeSet.h"
#include "game/abilities/effects/requirements/BaseGameplayEffectApplicationRequirement.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/component/SoulComponent.h"
#include "game/actor/item/Soul.h"
#include "game/actor/character/BaseCharacter.h"
#include "game/abilities/DungeonsAbilitySystemGlobals.h"
#include "game/abilities/effects/calculations/StatusModCalculations.h"

namespace {
	FName SpiritSpeedEffectDuration(TEXT("SwiftfootedEffectDuration"));
	FName SpiritSpeedEffectMagnitude(TEXT("SwiftfootedEffectMagnitude"));
}

USpiritSpeed::USpiritSpeed() {
	TypeId = EEnchantmentTypeID::SpiritSpeed;

	LevelMultiplier = [this](int level) -> float {
		return 1.0f + (2.0f * (float)level);
	};
	MultiplierFormatter = valueformat::asDurationRoundedSecond;
	Effect = USpiritSpeedGameplayEffect::StaticClass();
}

FText USpiritSpeed::CreateDescription() const {
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asPercentage(speedBonus)));
}

void USpiritSpeed::OnStart() {
	if (GetOwner()->HasAuthority()) {
		if (auto characterOwner = GetCharacterOwner()) {
			if (auto soulComponent = characterOwner->FindComponentByClass<USoulComponent>()) {
				soulComponent->OnAbsorbedSoul.AddUObject(this, &USpiritSpeed::OnGatheredSoul);
			}
		}
	}
}

void USpiritSpeed::OnGatheredSoul() {
	if (GetOwner()->HasAuthority()) {
		BoostSpeed();
	}
}

void USpiritSpeed::BoostSpeed() {
	if (auto abilitySystem = GetCharacterOwner()->GetAbilitySystemComponent()) {
		FParameterFilterContextWindow ParamWindow(EGameplayCueParametersField::EMPTY);
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpecFromSubClass(abilitySystem, Effect, Level);
		spec.SetSetByCallerMagnitude(::SpiritSpeedEffectMagnitude, speedBonus);
		spec.SetSetByCallerMagnitude(effects::DurationName, LevelMultiplier(Level));
		abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

USpiritSpeedGameplayEffect::USpiritSpeedGameplayEffect(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;

	FCustomCalculationBasedFloat durationMagnitude;
	durationMagnitude.CalculationClassMagnitude = UStatusDurationModCalculation::StaticClass();
	DurationMagnitude = durationMagnitude;

	StackLimitCount = 5;
	StackingType = EGameplayEffectStackingType::AggregateBySource;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	FGameplayModifierInfo info;
	info.Attribute = UMovementAttributeSet::SpeedMultiplierAttribute();

	FSetByCallerFloat speedMagnitude;
	speedMagnitude.DataName = ::SpiritSpeedEffectMagnitude;

	info.ModifierMagnitude = speedMagnitude;
	info.ModifierOp = EGameplayModOp::Type::Additive;

	Modifiers.Add(info);

	ApplicationRequirements.Emplace(UBaseGameplayEffectApplicationRequirement::StaticClass());

	InheritableGameplayEffectTags.AddTag(FGameplayTag::RequestGameplayTag("StatusEffect.Positive"));

	GameplayCues.Emplace(FGameplayTag::RequestGameplayTag("GameplayCue.StatusEffect.Enchantment.SpiritSpeed"), 0, 1);
}