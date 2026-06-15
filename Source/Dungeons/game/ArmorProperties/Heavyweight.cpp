// Fill out your copyright notice in the Description page of Project Settings.

#include "Dungeons.h"
#include "game/ArmorProperties/ArmorPropertyType.h"
#include "game/abilities/attributes/ResistanceAttributeSet.h"
#include "Heavyweight.h"
#include "GameplayEffectExtension.h"
#include "game/abilities/effects/GameplayEffectUtil.h"
#include "game/Enchantments/Enchantment.h"
#include <AbilitySystemGlobals.h>

// ----- HEAVYWEIGHT GAMEPLAY EFFECT ----- //
UHeavyweightGameplayEffect::UHeavyweightGameplayEffect() {
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	StackingType = EGameplayEffectStackingType::AggregateByTarget;
	StackDurationRefreshPolicy = EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;
	StackPeriodResetPolicy = EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
	bExecutePeriodicEffectOnApplication = false;
	bRequireModifierSuccessToTriggerCues = true;

	effects::AddSetByCallerAttribute(*this, UResistanceAttributeSet::PushbackResistanceMagnitudeAttribute(), EGameplayModOp::Division);
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag("Resistance.PushVolume.Wind"));
	effects::AddSetByCallerAttribute(*this, UResistanceAttributeSet::WindResistanceMagnitudeAttribute(), EGameplayModOp::Division);
}

// ----- HEAVYWEIGHT ARMOUR PROPERTY ----- //
void UHeavyweight::BeginPlay() {
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		float resistanceDivider = 1 / Math::max(0.01f, 1 - Resistance);
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UHeavyweightGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::PushbackResistanceMagnitudeAttribute().GetName(), resistanceDivider);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::WindResistanceMagnitudeAttribute().GetName(), resistanceDivider);
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UHeavyweight::EndPlay(const EEndPlayReason::Type reason) {
	Super::EndPlay(reason);
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}

UHeavyweight::UHeavyweight() {
	TypeID = EArmorPropertyID::Heavyweight;
}


// ----- HEAVYWEIGHT ENCHANTMENT ----- //

UEnchantmentHeavyweight::UEnchantmentHeavyweight()
{
	TypeId = EEnchantmentTypeID::Heavyweight;

	LevelMultiplier = [this](int level) -> float {
		return Resistance + PerLevelResistance * (1 - level);
	};
	MultiplierFormatter = valueformat::asPercentage;
}

FText UEnchantmentHeavyweight::CreateDescription() const
{
	return FText::Format(Super::CreateDescription(), FText::FromString(valueformat::asMultiplierPercentageChange(1 - Resistance)));
}

void UEnchantmentHeavyweight::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority) {
		auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

		FGameplayEffectSpec spec = effects::CreateGameplayEffectSpec<UHeavyweightGameplayEffect>(abilitySystem, 1.f);
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::PushbackResistanceMagnitudeAttribute().GetName(), 1 / Math::max(0.01, 1 - LevelMultiplier(Level)));
		spec.SetSetByCallerMagnitude(*UResistanceAttributeSet::WindResistanceMagnitudeAttribute().GetName(), 1 / Math::max(0.01, 1 - LevelMultiplier(Level)));
		Handle = abilitySystem->ApplyGameplayEffectSpecToSelf(spec);
	}
}

void UEnchantmentHeavyweight::EndPlay(const EEndPlayReason::Type)
{
	auto abilitySystem = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (Handle.IsValid() && abilitySystem) {
		abilitySystem->RemoveActiveGameplayEffect(Handle);
	}
}
